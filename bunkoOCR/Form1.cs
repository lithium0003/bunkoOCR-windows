using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace bunkoOCR
{
    public partial class Form1 : Form
    {
        private Stream _stream;
        private BinaryWriter _writer;
        private Process _process;
        private Dictionary<string, double> _dictionary;

        public Form1()
        {
            InitializeComponent();
            _dictionary = new Dictionary<string, double>();
            _dictionary["blank_cutoff"] = 35;
            _dictionary["ruby_cutoff"] = 0.5;
            _dictionary["rubybase_cutoff"] = 0.4;
            _dictionary["space_cutoff"] = 0.75;
            _dictionary["line_valueth"] = 0.5;
            _dictionary["detect_cut_off"] = 0.5;
            _dictionary["resize"] = 1;
            _dictionary["sleep_wait"] = 0;

            string[] lines;
            try
            {
                lines = File.ReadAllLines("param.config");
            }
            catch {
                lines = new string[0];
            }

            foreach (var line in lines)
            {
                // keyとvalueの区切り目を見つける
                var pos = line.IndexOf(':');
                if (pos < 0)
                {
                    continue;
                }

                // key-valueを一覧に追加
                var key = line.Substring(0, pos);
                var val = line.Substring(pos + 1);
                try
                {
                    double value = double.Parse(val);
                    _dictionary[key] = value;
                }
                catch { }
            }

            Task.Run(() =>
            {
                using (var process = new Process())
                {
                    _process = process;
                    process.StartInfo = new ProcessStartInfo()
                    {
                        FileName = "OCRengine.exe",
                        UseShellExecute = false,
                        CreateNoWindow = true,
                        RedirectStandardOutput = true,
                        RedirectStandardInput = true,
                        StandardOutputEncoding = Encoding.UTF8,
                    };

                    process.OutputDataReceived += OnStdOut;

                    process.Start();

                    process.BeginOutputReadLine();

                    _stream = process.StandardInput.BaseStream;
                    _writer = new BinaryWriter(_stream);

                    process.WaitForExit();

                    process.CancelOutputRead();
                    _process = null;
                }
            });

            Task.Run(async () =>
            {
                while(_stream == null)
                {
                    await Task.Delay(1000);
                }
                foreach (string key in _dictionary.Keys)
                {
                    var value = _dictionary[key];
                    var str = key + ":" + value.ToString() + "\r\n";
                    var b = Encoding.UTF8.GetBytes(str);
                    _writer.Write(b);
                    _writer.Flush();
                }
            });
        }

        private void process(string filename)
        {
            listBox1.Items.Add(filename);
            var b = Encoding.UTF8.GetBytes(filename + "\r\n");
            _writer.Write(b);
            _writer.Flush();
        }

        public void OnStdOut(object sender, DataReceivedEventArgs e)
        {
            if (string.IsNullOrEmpty(e.Data) || string.IsNullOrEmpty(e.Data.Trim()))
            {
                return;
            }
            if (e.Data.StartsWith("done: "))
            {
                var donefile = e.Data.Replace("done: ", "");
                if (listBox1.InvokeRequired)
                {
                    listBox1.Invoke((MethodInvoker)(() =>
                    {
                        listBox1.Items.Remove(donefile);
                    }));
                }
                else
                {
                    listBox1.Items.Remove(donefile);
                }
                if (listBox2.InvokeRequired)
                {
                    listBox2.Invoke((MethodInvoker)(() =>
                    {
                        listBox2.Items.Add(donefile);
                    }));
                }
                else
                {
                    listBox2.Items.Add(donefile);
                }
                Task.Run(() =>
                {
                    postprocess(donefile);
                });
            }
            if (e.Data.StartsWith("error: "))
            {
                var errorfile = e.Data.Replace("error: ", "");
                if (listBox1.InvokeRequired)
                {
                    listBox1.Invoke((MethodInvoker)(() =>
                    {
                        listBox1.Items.Remove(errorfile);
                    }));
                }
                else
                {
                    listBox1.Items.Remove(errorfile);
                }
                errorfile = "ERROR: " + errorfile;
                if (listBox2.InvokeRequired)
                {
                    listBox2.Invoke((MethodInvoker)(() =>
                    {
                        listBox2.Items.Add(errorfile);
                    }));
                }
                else
                {
                    listBox2.Items.Add(errorfile);
                }
            }
            if (textBox1.InvokeRequired)
            {
                textBox1.Invoke((MethodInvoker)(() =>
                {
                    textBox1.Text = e.Data;
                }));
            }
            else
            {
                textBox1.Text = e.Data;
            }
        }

        private void postprocess(string filename)
        {
            bool txtoutput = checkBox1.Checked;
            bool aozoraoutput = checkBox2.Checked;
            bool htmloutput = checkBox3.Checked;
            if(!txtoutput && !aozoraoutput && !htmloutput)
            {
                return;
            }
            string jsonfile = filename + ".json";
            if (File.Exists(jsonfile))
            {
                var jsonString = File.ReadAllText(jsonfile);
                var result = JsonSerializer.Deserialize<OCRresult>(jsonString);
                var str = result.text;

                if(txtoutput)
                {
                    var txtstr = Regex.Replace(str, "\uFFF9(.*?)\uFFFA(.*?)\uFFFB", "$1");
                    var txtfilename = filename + ".noruby.txt";
                    File.WriteAllText(txtfilename, txtstr);
                }
                if (aozoraoutput)
                {
                    var txtstr = Regex.Replace(str, "\uFFF9(.*?)\uFFFA(.*?)\uFFFB", "｜$1《$2》");
                    var txtfilename = filename + ".aozora.txt";
                    File.WriteAllText(txtfilename, txtstr);
                }
                if (htmloutput)
                {
                    var htmlstr = str;
                    htmlstr = htmlstr.Replace("&", "&amp;");
                    htmlstr = htmlstr.Replace("<", "&lt;");
                    htmlstr = htmlstr.Replace(">", "&gt;");
                    htmlstr = htmlstr.Replace("\n", "<br>\r\n");
                    htmlstr = Regex.Replace(htmlstr, "\uFFF9(.*?)\uFFFA(.*?)\uFFFB", "<ruby>$1<rt>$2</rt></ruby>");
                    htmlstr = "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<meta charset=\"utf-8\">\r\n</head>\r\n<body>\r\n" + htmlstr + "</body>\r\n</html>";
                    var htmlfilename = filename + ".html";
                    File.WriteAllText(htmlfilename, htmlstr);
                }
            }
        }
        private void button1_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                foreach (var file in openFileDialog1.FileNames)
                {
                    process(file);
                }
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if(listBox2.SelectedIndex >= 0)
            {
                string donefile = listBox2.SelectedItem.ToString();
                if(donefile.StartsWith("ERROR: "))
                {
                    return;
                }
                string jsonfile = donefile + ".json";
                if (File.Exists(jsonfile))
                {
                    var jsonString = File.ReadAllText(jsonfile);
                    var result = JsonSerializer.Deserialize<OCRresult>(jsonString);
                    var htmlstr = result.text;
                    htmlstr = htmlstr.Replace("&", "&amp;");
                    htmlstr = htmlstr.Replace("<", "&lt;");
                    htmlstr = htmlstr.Replace(">", "&gt;");
                    htmlstr = htmlstr.Replace("\n", "<br>\r\n");
                    htmlstr = Regex.Replace(htmlstr, "\uFFF9(.*?)\uFFFA(.*?)\uFFFB", "<ruby>$1<rt>$2</rt></ruby>");
                    htmlstr = "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<meta charset=\"utf-8\">\r\n</head>\r\n<body>\r\n" + htmlstr + "</body>\r\n</html>";
                    var form2 = new Form2();
                    form2.Text = donefile;
                    form2.load_text(htmlstr);
                    form2.Show();
                }
            }
        }

        private void Form1_DragDrop(object sender, DragEventArgs e)
        {
            string[] files = (string[])e.Data.GetData(DataFormats.FileDrop, false);
            foreach(var file in files)
            {
                process(file);
            }
        }

        private void Form1_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                e.Effect = DragDropEffects.All;
            }
            else
            {
                e.Effect = DragDropEffects.None;
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            using (StreamWriter writer = new StreamWriter("param.config"))
            {
                foreach (string key in _dictionary.Keys)
                {
                    var value = _dictionary[key];
                    writer.WriteLine(key + ":" + value);
                }
            }

            if (_process != null)
            {
                _process.Kill();
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            textBox2.Text = _dictionary[(string)comboBox1.SelectedItem].ToString();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            var param = (string)comboBox1.SelectedItem;
            if (string.IsNullOrEmpty(param)) { return; }
            try
            {
                var value = double.Parse(textBox2.Text);

                var str = param+":"+value.ToString();
                _dictionary[param] = value;
                var b = Encoding.UTF8.GetBytes(str + "\r\n");
                _writer.Write(b);
                _writer.Flush();
            }
            catch (Exception)
            {
                return;
            }
        }
    }

    public class Boxes
    {
        public int block { get; set; }
        public int line { get; set; }
        public int index { get; set; }
        public int direction { get; set; }
        public int ruby { get; set; }
        public int rubybase { get; set; }
        public int space { get; set; }
        public float cx { get; set; }
        public float cy { get; set; }
        public float w { get; set; }
        public float h { get; set; }
        public string character { get; set; }
    }

    public class OCRresult
    {
        public IList<Boxes> boxes { get; set; }
        public string text { get; set; }
    }
}

