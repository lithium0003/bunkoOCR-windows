using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace bunkoOCR
{
    public partial class Form3: Form
    {
        private Dictionary<string,double> dict = ConfigReader.Load();
        private Dictionary<string, string> dict2 = ConfigReader.LoadRubyTreat();
        private Dictionary<string, string> dict3 = ConfigReader.LoadPathSetting();

        public Form3()
        {
            InitializeComponent();
            LoadFromFile();
        }
        private void LoadFromFile()
        {
            checkBox_TensorRT.Checked = dict["use_TensorRT"] > 0;
            checkBox_CUDA.Checked = dict["use_CUDA"] > 0;
            checkBox_DML.Checked = dict["use_DirectML"] > 0;
            checkBox_OpneVINO.Checked = dict["use_OpenVINO"] > 0;
            int GPU_id = (int)dict["DML_GPU_id"];
            if(GPU_id < 0 || GPU_id > 254)
            {
                GPU_id = -1;
            }
            numericUpDown_GPUid.Value = GPU_id;

            checkBox_ausostart.Checked = dict["autostart"] > 0;

            numericUpDown_detect_cut_off.Value = (decimal)dict["detect_cut_off"];
            numericUpDown_blank_cutoff.Value = (decimal)dict["blank_cutoff"];
            numericUpDown_ruby_cutoff.Value = (decimal)dict["ruby_cutoff"];
            numericUpDown_rubybase_cutoff.Value = (decimal)dict["rubybase_cutoff"];
            numericUpDown_space_cutoff.Value = (decimal)dict["space_cutoff"];
            numericUpDown_emphasis_cutoff.Value = (decimal)dict["emphasis_cutoff"];
            numericUpDown_line_valueth.Value = (decimal)dict["line_valueth"];
            numericUpDown_sep_valueth.Value = (decimal)dict["sep_valueth"];
            numericUpDown_sep_valueth2.Value = (decimal)dict["sep_valueth2"];
            numericUpDown_allowwidth_next.Value = (decimal)dict["allowwidth_next"];
            numericUpDown_resize.Value = (decimal)dict["resize"];
            numericUpDown_sleep_wait.Value = (decimal)dict["sleep_wait"];

            checkBox_rawoutput.Checked = dict2["raw_output"] == "1";
            checkBox_output_text.Checked = dict2["output_text"] == "1";
            textBox_rubybefore.Text = dict2["before_ruby"];
            textBox_rubysepatator.Text = dict2["separator_ruby"];
            checkBox_output_ruby.Checked = dict2["output_ruby"] == "1";
            textBox_rubyafter.Text = dict2["after_ruby"];
            textBox_rubybefore.Enabled = false;
            textBox_rubysepatator.Enabled = false;
            textBox_rubyafter.Enabled = false;
            checkBox_output_ruby.Enabled = false;
            if (dict2["output_ruby"] == "0" && dict2["before_ruby"] == "" && dict2["separator_ruby"] == "" && dict2["after_ruby"] == "")
            {
                radioButton_no_ruby.Checked = true;
            }
            else if(dict2["output_ruby"] == "1" && dict2["before_ruby"] == "｜" && dict2["separator_ruby"] == "《" && dict2["after_ruby"] == "》")
            {
                radioButton_aozora.Checked = true;
            }
            else
            {
                radioButton_custum.Checked = true;
                textBox_rubybefore.Enabled = true;
                textBox_rubysepatator.Enabled = true;
                textBox_rubyafter.Enabled = true;
                checkBox_output_ruby.Enabled = true;
            }
            if(checkBox_rawoutput.Checked)
            {
                radioButton_no_ruby.Enabled = false;
                radioButton_aozora.Enabled = false;
                radioButton_custum.Enabled = false;

                textBox_rubybefore.Enabled = false;
                textBox_rubysepatator.Enabled = false;
                textBox_rubyafter.Enabled = false;
                checkBox_output_ruby.Enabled = false;
            }

            print_label_rubysample();

            textBox_outputdir.Text = dict3["output_dir"];
            if (dict3["override"] == "1")
            {
                radioButton_override.Checked = true;
            }
            else
            {
                radioButton_addnumber.Checked = true;
            }
        }

        private void print_label_rubysample()
        {
            if(checkBox_rawoutput.Checked)
            {
                label_rubysample.Text = "";
            }
            else if (!checkBox_output_ruby.Checked)
            {
                label_rubysample.Text = "とある科学の" + textBox_rubybefore.Text + "超電磁砲" + textBox_rubyafter.Text;
            }
            else
            {
                label_rubysample.Text = "とある科学の" + textBox_rubybefore.Text + "超電磁砲" + textBox_rubysepatator.Text + "レールガン" + textBox_rubyafter.Text;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            dict["use_TensorRT"] = checkBox_TensorRT.Checked ? 1 : 0;
            dict["use_CUDA"] = checkBox_CUDA.Checked ? 1 : 0;
            dict["use_DirectML"] = checkBox_DML.Checked ? 1 : 0;
            dict["use_OpenVINO"] = checkBox_OpneVINO.Checked ? 1 : 0;
            dict["DML_GPU_id"] = (double)numericUpDown_GPUid.Value;

            dict["autostart"] = checkBox_ausostart.Checked ? 1 : 0;

            dict["detect_cut_off"] = (double)numericUpDown_detect_cut_off.Value;
            dict["blank_cutoff"] = (double)numericUpDown_blank_cutoff.Value;
            dict["ruby_cutoff"] = (double)numericUpDown_ruby_cutoff.Value;
            dict["rubybase_cutoff"] = (double)numericUpDown_rubybase_cutoff.Value;
            dict["space_cutoff"] = (double)numericUpDown_space_cutoff.Value;
            dict["emphasis_cutoff"] = (double)numericUpDown_emphasis_cutoff.Value;
            dict["line_valueth"] = (double)numericUpDown_line_valueth.Value;
            dict["sep_valueth"] = (double)numericUpDown_sep_valueth.Value;
            dict["sep_valueth2"] = (double)numericUpDown_sep_valueth2.Value;
            dict["allowwidth_next"] = (double)numericUpDown_allowwidth_next.Value;
            dict["resize"] = (double)numericUpDown_resize.Value;
            dict["sleep_wait"] = (double)numericUpDown_sleep_wait.Value;

            dict2["raw_output"] = checkBox_rawoutput.Checked ? "1" : "0";
            dict2["output_text"] = checkBox_output_text.Checked ? "1" : "0";
            dict2["before_ruby"] = textBox_rubybefore.Text;
            dict2["separator_ruby"] = textBox_rubysepatator.Text;
            dict2["after_ruby"] = textBox_rubyafter.Text;
            dict2["output_ruby"] = checkBox_output_ruby.Checked ? "1" : "0";

            dict3["output_dir"] = textBox_outputdir.Text;
            dict3["override"] = radioButton_override.Checked ? "1" : "0";

            ConfigReader.Save(dict);
            ConfigReader.SaveRubyTreat(dict2);
            ConfigReader.SavePathSetting(dict3);
        }

        private void radioButton_no_ruby_CheckedChanged(object sender, EventArgs e)
        {
            if(radioButton_no_ruby.Checked)
            {
                textBox_rubybefore.Text = "";
                textBox_rubysepatator.Text = "";
                textBox_rubyafter.Text = "";
                checkBox_output_ruby.Checked = false;

                textBox_rubybefore.Enabled = false;
                textBox_rubysepatator.Enabled = false;
                textBox_rubyafter.Enabled = false;
                checkBox_output_ruby.Enabled = false;
                print_label_rubysample();
            }
        }

        private void radioButton_aozora_CheckedChanged(object sender, EventArgs e)
        {
            if (radioButton_aozora.Checked)
            {
                textBox_rubybefore.Text = "｜";
                textBox_rubysepatator.Text = "《";
                textBox_rubyafter.Text = "》";
                checkBox_output_ruby.Checked = true;

                textBox_rubybefore.Enabled = false;
                textBox_rubysepatator.Enabled = false;
                textBox_rubyafter.Enabled = false;
                checkBox_output_ruby.Enabled = false;
                print_label_rubysample();
            }
        }

        private void radioButton_custum_CheckedChanged(object sender, EventArgs e)
        {
            if (radioButton_custum.Checked)
            {
                textBox_rubybefore.Enabled = true;
                textBox_rubysepatator.Enabled = true;
                textBox_rubyafter.Enabled = true;
                checkBox_output_ruby.Enabled = true;
            }
        }

        private void checkBox_output_ruby_CheckedChanged(object sender, EventArgs e)
        {
            print_label_rubysample();
        }

        private void textBox_rubybefore_TextChanged(object sender, EventArgs e)
        {
            print_label_rubysample();
        }

        private void textBox_rubysepatator_TextChanged(object sender, EventArgs e)
        {
            print_label_rubysample();
        }

        private void textBox_rubyafter_TextChanged(object sender, EventArgs e)
        {
            print_label_rubysample();
        }

        private void checkBox_rawoutput_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox_rawoutput.Checked)
            {
                radioButton_no_ruby.Enabled = false;
                radioButton_aozora.Enabled = false;
                radioButton_custum.Enabled = false;

                textBox_rubybefore.Enabled = false;
                textBox_rubysepatator.Enabled = false;
                textBox_rubyafter.Enabled = false;
                checkBox_output_ruby.Enabled = false;
            }
            else
            {
                radioButton_no_ruby.Enabled = true;
                radioButton_aozora.Enabled = true;
                radioButton_custum.Enabled = true;

                if(radioButton_custum.Checked)
                {
                    textBox_rubybefore.Enabled = true;
                    textBox_rubysepatator.Enabled = true;
                    textBox_rubyafter.Enabled = true;
                    checkBox_output_ruby.Enabled = true;
                }
            }

            print_label_rubysample();
        }

        private void button_select_outputdir_Click(object sender, EventArgs e)
        {
            folderBrowserDialog1.SelectedPath = textBox_outputdir.Text;
            if(folderBrowserDialog1.ShowDialog() == DialogResult.OK)
            {
                textBox_outputdir.Text = folderBrowserDialog1.SelectedPath;
            }
        }
    }
}
