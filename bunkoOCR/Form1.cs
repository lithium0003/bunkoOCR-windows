using System;
using System.Collections;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Encodings.Web;
using System.Text.Json;
using System.Text.RegularExpressions;
using System.Text.Unicode;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace bunkoOCR
{
    public partial class Form1 : Form
    {
        private Stream _stream;
        private BinaryWriter _writer;
        private Process _process;
        private ConcurrentQueue<byte[]> _queue;
        private ConcurrentDictionary<string, string> _output_list = new ConcurrentDictionary<string, string>();

        bool ready = false;
        bool restart = false;

        void InitializeProcess()
        {
            var dict = ConfigReader.Load();

            bool autostart = dict["autostart"] > 0;
            if (button_start.InvokeRequired)
            {
                button_start.Invoke((MethodInvoker)(() =>
                {
                    button_start.Enabled = !autostart;
                }));
            }
            else
            {
                button_start.Enabled = !autostart;
            }

            bool useTensorRT = dict["use_TensorRT"] > 0;
            bool useCUDA = dict["use_CUDA"] > 0;
            bool useDML = dict["use_DirectML"] > 0;
            int GPU_id = (int)dict["DML_GPU_id"];
            if (GPU_id < 0 || GPU_id > 254)
            {
                GPU_id = -1;
            }
            bool useOpenVINO = dict["use_OpenVINO"] > 0;
            dict = ConfigReader.FilterForOCRengine(dict);

            _queue = new ConcurrentQueue<byte[]>();

            Task.Run(() =>
            {
                if (useDML && GPU_id < 0)
                {
                    using (var process = new Process())
                    {
                        process.StartInfo = new ProcessStartInfo()
                        {
                            FileName = "detectGPU.exe",
                            UseShellExecute = false,
                            CreateNoWindow = true,
                        };

                        process.Start();
                        process.WaitForExit();
                        GPU_id = process.ExitCode;
                    }
                }

                string arg = "";
                if (useTensorRT)
                {
                    arg += "TensorRT ";
                }
                if (useCUDA)
                {
                    arg += "CUDA ";
                }
                if (useOpenVINO)
                {
                    arg += "OpenVINO ";
                }
                if (useDML && GPU_id != 255 && GPU_id >= 0)
                {
                    arg += GPU_id.ToString();
                }

                Task.Run(async () =>
                {
                    while (!ready)
                    {
                        await Task.Delay(1000);
                    }
                    while (_process != null)
                    {
                        if (_queue.TryDequeue(out var b))
                        {
                            if (b == null) break;
                            _writer.Write(b);
                            _writer.Flush();
                        }
                        else
                        {
                            await Task.Delay(1000);
                        }
                    }
                });

                Task.Run(async () =>
                {
                    while (_stream == null)
                    {
                        await Task.Delay(1000);
                    }
                    var str = "";
                    foreach (string key in dict.Keys)
                    {
                        var value = dict[key];
                        str += key + ":" + value.ToString() + "\r\n";
                    }
                    var b = Encoding.UTF8.GetBytes(str);
                    _queue.Enqueue(b);
                });

                using (var process = new Process())
                {
                    _process = process;
                    ready = true;
                    process.StartInfo = new ProcessStartInfo()
                    {
                        FileName = "OCRengine.exe",
                        UseShellExecute = false,
                        CreateNoWindow = true,
                        RedirectStandardOutput = true,
                        RedirectStandardInput = true,
                        StandardOutputEncoding = Encoding.UTF8,
                        Arguments = arg,
                    };

                    process.OutputDataReceived += OnStdOut;

                    process.Start();

                    process.BeginOutputReadLine();

                    _stream = process.StandardInput.BaseStream;
                    _writer = new BinaryWriter(_stream);

                    process.WaitForExit();
                    _queue.Enqueue(null);

                    process.CancelOutputRead();

                    _process = null;
                    _stream = null;
                    ready = false;
                    if(restart)
                    {
                        restart = false;
                        InitializeProcess();
                    }
                }
            });
        }

        public Form1()
        {
            InitializeComponent();
            InitializeProcess();
        }

        private bool SendToEngine(string filename)
        {
            var dict = ConfigReader.LoadPathSetting();
            var output_dir = dict["output_dir"];
            var override_flag = dict["override"] == "1";
            var output_filename = "";

            if(output_dir != "")
            {
                Directory.CreateDirectory(output_dir);
                output_filename = Path.Combine(output_dir, Path.GetFileName(filename) + ".json");

                if(File.Exists(output_filename) && !override_flag)
                {
                    var count = 1;
                    while(File.Exists(output_filename))
                    {
                        output_filename = Path.Combine(output_dir, Path.GetFileName(filename) + "." + count.ToString() + ".json");
                        count += 1;
                    }
                }
            }
            else if(!override_flag)
            {
                output_filename = filename + ".json";

                if (File.Exists(output_filename))
                {
                    var count = 1;
                    while (File.Exists(output_filename))
                    {
                        output_filename = filename + "." + count.ToString() + ".json";
                        count += 1;
                    }
                }
            }

            if(!_output_list.TryAdd(filename, output_filename))
            {
                return false;
            }
            var b = Encoding.UTF8.GetBytes(filename + "\r\n" + output_filename + "\r\n");
            _queue.Enqueue(b);
            return true;
        }

        private void process(string filename)
        {
            button_config.Enabled = false;
            if(button_start.Enabled)
            {
                if(!listBox1.Items.Contains(filename))
                {
                    listBox1.Items.Add(filename);
                }
            }
            else
            {
                if(SendToEngine(filename))
                {
                    listBox1.Items.Add(filename);
                }
            }
        }

        public void OnStdOut(object sender, DataReceivedEventArgs e)
        {
            Debug.WriteLine(e.Data);
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
                if (_output_list.TryRemove(donefile, out var jsonfile))
                {
                    Task.Run(() =>
                    {
                        postprocess(donefile, jsonfile);

                        if (listBox2.InvokeRequired)
                        {
                            listBox2.Invoke((MethodInvoker)(() =>
                            {
                                listBox2.Items.Add(jsonfile);
                            }));
                        }
                        else
                        {
                            listBox2.Items.Add(jsonfile);
                        }
                    });
                }
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
                _output_list.TryRemove(errorfile, out var jsonfile);
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
                    var lines = textBox1.Lines.ToList();
                    lines.Add(e.Data);
                    textBox1.Lines = lines.ToArray();
                    textBox1.SelectionStart = textBox1.Text.Length;
                    textBox1.ScrollToCaret();
                }));
            }
            else
            {
                var lines = textBox1.Lines.ToList();
                lines.Add(e.Data);
                textBox1.Lines = lines.ToArray();
                textBox1.SelectionStart = textBox1.Text.Length;
                textBox1.ScrollToCaret();
            }
            if (listBox1.Items.Count == 0)
            {
                if(button_config.InvokeRequired)
                {
                    button_config.Invoke((MethodInvoker)(() =>
                    {
                        button_config.Enabled = true;
                    }));
                }
                else
                {
                    button_config.Enabled = true;
                }
            }
        }

        private void postprocess(string filename, string jsonfile)
        {
            var dict = ConfigReader.LoadRubyTreat();

            var raw_output = dict["raw_output"] == "1";
            var output_text = dict["output_text"] == "1";
            var output_ruby = dict["output_ruby"] == "1";
            var before_ruby = dict["before_ruby"];
            var separator_ruby = dict["separator_ruby"];
            var after_ruby = dict["after_ruby"];

            string process_string(string target)
            {
                if(output_ruby)
                {
                    return Regex.Replace(target, "\uFFF9(.*?)\uFFFA(.*?)\uFFFB", before_ruby + "$1" + separator_ruby + "$2" + after_ruby);
                }
                else
                {
                    return Regex.Replace(target, "\uFFF9(.*?)\uFFFA(.*?)\uFFFB", before_ruby + "$1" + after_ruby);
                }
            }

            if (jsonfile == "")
            {
                jsonfile = filename + ".json";
            }

            if (File.Exists(jsonfile))
            {
                var jsonString = File.ReadAllText(jsonfile);
                var result = JsonSerializer.Deserialize<OCRresult>(jsonString);

                if(!raw_output)
                {
                    result.text = process_string(result.text);
                    result.block = result.block.Select(item => {
                        item.text = process_string(item.text);
                        return item;
                    }).ToList();
                    result.line = result.line.Select(item => {
                        item.text = process_string(item.text);
                        return item;
                    }).ToList();
                }

                var options = new JsonSerializerOptions {
                    Encoder = JavaScriptEncoder.Create(UnicodeRanges.All),
                    WriteIndented = true
                };
                var jsonUtf8Bytes = JsonSerializer.SerializeToUtf8Bytes(result, options);
                File.WriteAllBytes(jsonfile, jsonUtf8Bytes);

                if(output_text)
                {
                    var txtfilename = Path.ChangeExtension(jsonfile, ".txt");
                    File.WriteAllText(txtfilename, result.text);
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
                string jsonfile = listBox2.SelectedItem.ToString();
                if(jsonfile.StartsWith("ERROR: "))
                {
                    return;
                }
                if (File.Exists(jsonfile))
                {
                    var jsonString = File.ReadAllText(jsonfile, Encoding.UTF8);
                    var result = JsonSerializer.Deserialize<OCRresult>(jsonString);
                    var htmlstr = result.text;
                    htmlstr = htmlstr.Replace("&", "&amp;");
                    htmlstr = htmlstr.Replace("<", "&lt;");
                    htmlstr = htmlstr.Replace(">", "&gt;");
                    htmlstr = htmlstr.Replace("\n", "<br>\r\n");
                    htmlstr = Regex.Replace(htmlstr, "\uFFF9(.*?)\uFFFA(.*?)\uFFFB", "<ruby>$1<rt>$2</rt></ruby>");
                    htmlstr = "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<meta charset=\"utf-8\">\r\n</head>\r\n<body>\r\n" + htmlstr + "</body>\r\n</html>";
                    var form2 = new Form2();
                    form2.Text = jsonfile;
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
            if (_process != null)
            {
                _process.Kill();
            }
        }

        private void ReloadEngine()
        {
            listBox1.Items.Clear();
            if (_process != null)
            {
                restart = true;
                _process.Kill();
            }
            else
            {
                InitializeProcess();
            }
        }

        private void button_config_Click(object sender, EventArgs e)
        {
            var form3 = new Form3();
            if(form3.ShowDialog() == DialogResult.OK)
            {
                ReloadEngine();
            }
        }

        private void button_kill_Click(object sender, EventArgs e)
        {
            ReloadEngine();
        }

        private void button_start_Click(object sender, EventArgs e)
        {
            foreach(string filename in listBox1.Items) {
                SendToEngine(filename);
            }
        }
    }

    public class Block
    {
        public int blockidx { get; set; }
        public string text { get; set; }
        public int vertical { get; set; }
        public float x1 { get; set; }
        public float x2 { get; set; }
        public float y1 { get; set; }
        public float y2 { get; set; }
    }

    public class Box
    {
        public int blockidx { get; set; }
        public int lineidx { get; set; }
        public int subidx { get; set; }
        public int vertical { get; set; }
        public int ruby { get; set; }
        public int rubybase { get; set; }
        public int emphasis { get; set; }
        public float cx { get; set; }
        public float cy { get; set; }
        public float w { get; set; }
        public float h { get; set; }
        public string text { get; set; }
    }
    public class Line
    {
        public int blockidx { get; set; }
        public int lineidx { get; set; }
        public int vertical { get; set; }
        public float x1 { get; set; }
        public float x2 { get; set; }
        public float y1 { get; set; }
        public float y2 { get; set; }
        public string text { get; set; }
    }

    public class OCRresult
    {
        public List<Block> block { get; set; }
        public List<Box> box { get; set; }
        public List<Line> line { get; set; }
        public string text { get; set; }
    }
}

