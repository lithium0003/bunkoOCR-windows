using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace bunkoOCR
{
    class ConfigReader
    {
        static public Dictionary<string, double> FilterForOCRengine(Dictionary<string, double> dict)
        {
            dict.Remove("use_TensorRT");
            dict.Remove("use_CUDA");
            dict.Remove("use_DirectML");
            dict.Remove("use_OpenVINO");
            dict.Remove("DML_GPU_id");

            dict.Remove("autostart");
            return dict;
        }


        static public Dictionary<string, double> Load()
        {
            var dictionary = new Dictionary<string, double>
            {
                ["detect_cut_off"] = 0.35,
                ["blank_cutoff"] = 20,
                ["ruby_cutoff"] = 0.25,
                ["rubybase_cutoff"] = 0.75,
                ["space_cutoff"] = 0.5,
                ["emphasis_cutoff"] = 0.5,
                ["line_valueth"] = 0.25,
                ["sep_valueth"] = 0.15,
                ["sep_valueth2"] = 0.2,
                ["allowwidth_next"] = 1.0,
                ["resize"] = 1,
                ["sleep_wait"] = 0,

                ["use_TensorRT"] = 1,
                ["use_CUDA"] = 1,
                ["use_DirectML"] = 1,
                ["DML_GPU_id"] = -1,
                ["use_OpenVINO"] = 1,

                ["autostart"] = 1,
            };

            string[] lines;
            try
            {
                lines = File.ReadAllLines("param.config");
            }
            catch
            {
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
                    dictionary[key] = value;
                }
                catch { }
            }

            return dictionary;
        }

        static public void Save(Dictionary<string, double> dict)
        {
            using (StreamWriter writer = new StreamWriter("param.config"))
            {
                foreach (string key in dict.Keys)
                {
                    var value = dict[key];
                    writer.WriteLine(key + ":" + value);
                }
            }
        }

        static public Dictionary<string, string> LoadRubyTreat()
        {
            var dictionary = new Dictionary<string, string>
            {
                ["raw_output"] = "0",
                ["output_text"] = "1",
                ["output_ruby"] = "1",
                ["before_ruby"] = "｜",
                ["separator_ruby"] = "《",
                ["after_ruby"] = "》"
            };

            string[] lines;
            try
            {
                lines = File.ReadAllLines("ruby.config");
            }
            catch
            {
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
                dictionary[key] = val;
            }

            return dictionary;
        }

        static public void SaveRubyTreat(Dictionary<string, string> dict)
        {
            using (StreamWriter writer = new StreamWriter("ruby.config"))
            {
                foreach (string key in dict.Keys)
                {
                    var value = dict[key];
                    writer.WriteLine(key + ":" + value);
                }
            }
        }


        static public Dictionary<string, string> LoadPathSetting()
        {
            var dictionary = new Dictionary<string, string>
            {
                ["output_dir"] = "",
                ["override"] = "0",
            };

            string[] lines;
            try
            {
                lines = File.ReadAllLines("path.config");
            }
            catch
            {
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
                dictionary[key] = val;
            }

            return dictionary;
        }

        static public void SavePathSetting(Dictionary<string, string> dict)
        {
            using (StreamWriter writer = new StreamWriter("path.config"))
            {
                foreach (string key in dict.Keys)
                {
                    var value = dict[key];
                    writer.WriteLine(key + ":" + value);
                }
            }
        }
    }
}