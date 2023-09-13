using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace jsonToText
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void postprocess(string jsonfile)
        {
            bool txtoutput = checkBox1.Checked;
            bool aozoraoutput = checkBox2.Checked;
            bool htmloutput = checkBox3.Checked;
            if (!txtoutput && !aozoraoutput && !htmloutput)
            {
                return;
            }
            var filename = jsonfile.Replace(".json", "");
            if (File.Exists(jsonfile))
            {
                var jsonString = File.ReadAllText(jsonfile);
                var result = JsonSerializer.Deserialize<OCRresult>(jsonString);
                var str = result.text;

                if (txtoutput)
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
                    enqueue(file);
                }
                process();
            }
        }

        private void Form1_DragDrop(object sender, DragEventArgs e)
        {
            string[] files = (string[])e.Data.GetData(DataFormats.FileDrop, false);
            foreach (var file in files)
            {
                enqueue(file);
            }
            process();
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

        private void enqueue(string filename)
        {
            listBox1.Items.Add(filename);
        }

        private void process()
        {
            var files = listBox1.Items.Cast<string>().ToArray();
            Task.Run(() =>
            {
                foreach (var file in files)
                {
                    if (listBox1.InvokeRequired)
                    {
                        listBox1.Invoke((MethodInvoker)(() =>
                        {
                            listBox1.Items.Remove(file);
                        }));
                    }
                    else
                    {
                        listBox1.Items.Remove(file);
                    }
                    postprocess(file);
                    if (listBox2.InvokeRequired)
                    {
                        listBox2.Invoke((MethodInvoker)(() =>
                        {
                            listBox2.Items.Add(file);
                        }));
                    }
                    else
                    {
                        listBox2.Items.Add(file);
                    }
                }
            });
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
