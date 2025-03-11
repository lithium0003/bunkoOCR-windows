namespace bunkoOCR
{
    partial class Form3
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.button1 = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.checkBox_OpneVINO = new System.Windows.Forms.CheckBox();
            this.label1 = new System.Windows.Forms.Label();
            this.numericUpDown_GPUid = new System.Windows.Forms.NumericUpDown();
            this.checkBox_DML = new System.Windows.Forms.CheckBox();
            this.checkBox_CUDA = new System.Windows.Forms.CheckBox();
            this.checkBox_TensorRT = new System.Windows.Forms.CheckBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.numericUpDown_allowwidth_next = new System.Windows.Forms.NumericUpDown();
            this.label11 = new System.Windows.Forms.Label();
            this.numericUpDown_sep_valueth2 = new System.Windows.Forms.NumericUpDown();
            this.label10 = new System.Windows.Forms.Label();
            this.numericUpDown_sep_valueth = new System.Windows.Forms.NumericUpDown();
            this.label9 = new System.Windows.Forms.Label();
            this.numericUpDown_line_valueth = new System.Windows.Forms.NumericUpDown();
            this.label8 = new System.Windows.Forms.Label();
            this.numericUpDown_emphasis_cutoff = new System.Windows.Forms.NumericUpDown();
            this.label7 = new System.Windows.Forms.Label();
            this.numericUpDown_space_cutoff = new System.Windows.Forms.NumericUpDown();
            this.label6 = new System.Windows.Forms.Label();
            this.numericUpDown_rubybase_cutoff = new System.Windows.Forms.NumericUpDown();
            this.label5 = new System.Windows.Forms.Label();
            this.numericUpDown_ruby_cutoff = new System.Windows.Forms.NumericUpDown();
            this.label4 = new System.Windows.Forms.Label();
            this.numericUpDown_blank_cutoff = new System.Windows.Forms.NumericUpDown();
            this.label3 = new System.Windows.Forms.Label();
            this.numericUpDown_detect_cut_off = new System.Windows.Forms.NumericUpDown();
            this.label2 = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.label14 = new System.Windows.Forms.Label();
            this.numericUpDown_sleep_wait = new System.Windows.Forms.NumericUpDown();
            this.label13 = new System.Windows.Forms.Label();
            this.numericUpDown_resize = new System.Windows.Forms.NumericUpDown();
            this.label12 = new System.Windows.Forms.Label();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.label_rubysample = new System.Windows.Forms.Label();
            this.label17 = new System.Windows.Forms.Label();
            this.textBox_rubyafter = new System.Windows.Forms.TextBox();
            this.label16 = new System.Windows.Forms.Label();
            this.textBox_rubysepatator = new System.Windows.Forms.TextBox();
            this.label15 = new System.Windows.Forms.Label();
            this.textBox_rubybefore = new System.Windows.Forms.TextBox();
            this.radioButton_custum = new System.Windows.Forms.RadioButton();
            this.radioButton_aozora = new System.Windows.Forms.RadioButton();
            this.radioButton_no_ruby = new System.Windows.Forms.RadioButton();
            this.checkBox_output_text = new System.Windows.Forms.CheckBox();
            this.checkBox_output_ruby = new System.Windows.Forms.CheckBox();
            this.checkBox_rawoutput = new System.Windows.Forms.CheckBox();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_GPUid)).BeginInit();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_allowwidth_next)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_sep_valueth2)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_sep_valueth)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_line_valueth)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_emphasis_cutoff)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_space_cutoff)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_rubybase_cutoff)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_ruby_cutoff)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_blank_cutoff)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_detect_cut_off)).BeginInit();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_sleep_wait)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_resize)).BeginInit();
            this.groupBox4.SuspendLayout();
            this.SuspendLayout();
            // 
            // button1
            // 
            this.button1.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.button1.Font = new System.Drawing.Font("MS UI Gothic", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.button1.Location = new System.Drawing.Point(540, 445);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(135, 67);
            this.button1.TabIndex = 0;
            this.button1.Text = "OK";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.checkBox_OpneVINO);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.numericUpDown_GPUid);
            this.groupBox1.Controls.Add(this.checkBox_DML);
            this.groupBox1.Controls.Add(this.checkBox_CUDA);
            this.groupBox1.Controls.Add(this.checkBox_TensorRT);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(283, 150);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Acceralator";
            // 
            // checkBox_OpneVINO
            // 
            this.checkBox_OpneVINO.AutoSize = true;
            this.checkBox_OpneVINO.Location = new System.Drawing.Point(161, 24);
            this.checkBox_OpneVINO.Name = "checkBox_OpneVINO";
            this.checkBox_OpneVINO.Size = new System.Drawing.Size(114, 22);
            this.checkBox_OpneVINO.TabIndex = 5;
            this.checkBox_OpneVINO.Text = "OpenVINO";
            this.checkBox_OpneVINO.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(124, 104);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(61, 18);
            this.label1.TabIndex = 4;
            this.label1.Text = "GPU id";
            // 
            // numericUpDown_GPUid
            // 
            this.numericUpDown_GPUid.Location = new System.Drawing.Point(218, 97);
            this.numericUpDown_GPUid.Maximum = new decimal(new int[] {
            254,
            0,
            0,
            0});
            this.numericUpDown_GPUid.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            -2147483648});
            this.numericUpDown_GPUid.Name = "numericUpDown_GPUid";
            this.numericUpDown_GPUid.Size = new System.Drawing.Size(57, 25);
            this.numericUpDown_GPUid.TabIndex = 3;
            this.numericUpDown_GPUid.Value = new decimal(new int[] {
            1,
            0,
            0,
            -2147483648});
            // 
            // checkBox_DML
            // 
            this.checkBox_DML.AutoSize = true;
            this.checkBox_DML.Location = new System.Drawing.Point(15, 104);
            this.checkBox_DML.Name = "checkBox_DML";
            this.checkBox_DML.Size = new System.Drawing.Size(103, 22);
            this.checkBox_DML.TabIndex = 2;
            this.checkBox_DML.Text = "DirectML";
            this.checkBox_DML.UseVisualStyleBackColor = true;
            // 
            // checkBox_CUDA
            // 
            this.checkBox_CUDA.AutoSize = true;
            this.checkBox_CUDA.Location = new System.Drawing.Point(15, 60);
            this.checkBox_CUDA.Name = "checkBox_CUDA";
            this.checkBox_CUDA.Size = new System.Drawing.Size(81, 22);
            this.checkBox_CUDA.TabIndex = 1;
            this.checkBox_CUDA.Text = "CUDA";
            this.checkBox_CUDA.UseVisualStyleBackColor = true;
            // 
            // checkBox_TensorRT
            // 
            this.checkBox_TensorRT.AutoSize = true;
            this.checkBox_TensorRT.Location = new System.Drawing.Point(15, 24);
            this.checkBox_TensorRT.Name = "checkBox_TensorRT";
            this.checkBox_TensorRT.Size = new System.Drawing.Size(108, 22);
            this.checkBox_TensorRT.TabIndex = 0;
            this.checkBox_TensorRT.Text = "TensorRT";
            this.checkBox_TensorRT.UseVisualStyleBackColor = true;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.numericUpDown_allowwidth_next);
            this.groupBox2.Controls.Add(this.label11);
            this.groupBox2.Controls.Add(this.numericUpDown_sep_valueth2);
            this.groupBox2.Controls.Add(this.label10);
            this.groupBox2.Controls.Add(this.numericUpDown_sep_valueth);
            this.groupBox2.Controls.Add(this.label9);
            this.groupBox2.Controls.Add(this.numericUpDown_line_valueth);
            this.groupBox2.Controls.Add(this.label8);
            this.groupBox2.Controls.Add(this.numericUpDown_emphasis_cutoff);
            this.groupBox2.Controls.Add(this.label7);
            this.groupBox2.Controls.Add(this.numericUpDown_space_cutoff);
            this.groupBox2.Controls.Add(this.label6);
            this.groupBox2.Controls.Add(this.numericUpDown_rubybase_cutoff);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.numericUpDown_ruby_cutoff);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.numericUpDown_blank_cutoff);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Controls.Add(this.numericUpDown_detect_cut_off);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Location = new System.Drawing.Point(12, 176);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(277, 336);
            this.groupBox2.TabIndex = 2;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Detection parameter";
            // 
            // numericUpDown_allowwidth_next
            // 
            this.numericUpDown_allowwidth_next.DecimalPlaces = 2;
            this.numericUpDown_allowwidth_next.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.numericUpDown_allowwidth_next.Location = new System.Drawing.Point(170, 297);
            this.numericUpDown_allowwidth_next.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.numericUpDown_allowwidth_next.Name = "numericUpDown_allowwidth_next";
            this.numericUpDown_allowwidth_next.Size = new System.Drawing.Size(90, 25);
            this.numericUpDown_allowwidth_next.TabIndex = 19;
            this.numericUpDown_allowwidth_next.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(6, 304);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(123, 18);
            this.label11.TabIndex = 18;
            this.label11.Text = "allowwidth_next";
            // 
            // numericUpDown_sep_valueth2
            // 
            this.numericUpDown_sep_valueth2.DecimalPlaces = 2;
            this.numericUpDown_sep_valueth2.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.numericUpDown_sep_valueth2.Location = new System.Drawing.Point(170, 267);
            this.numericUpDown_sep_valueth2.Maximum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDown_sep_valueth2.Name = "numericUpDown_sep_valueth2";
            this.numericUpDown_sep_valueth2.Size = new System.Drawing.Size(90, 25);
            this.numericUpDown_sep_valueth2.TabIndex = 17;
            this.numericUpDown_sep_valueth2.Value = new decimal(new int[] {
            2,
            0,
            0,
            65536});
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(6, 274);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(103, 18);
            this.label10.TabIndex = 16;
            this.label10.Text = "sep_valueth2";
            // 
            // numericUpDown_sep_valueth
            // 
            this.numericUpDown_sep_valueth.DecimalPlaces = 2;
            this.numericUpDown_sep_valueth.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.numericUpDown_sep_valueth.Location = new System.Drawing.Point(170, 236);
            this.numericUpDown_sep_valueth.Maximum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDown_sep_valueth.Name = "numericUpDown_sep_valueth";
            this.numericUpDown_sep_valueth.Size = new System.Drawing.Size(90, 25);
            this.numericUpDown_sep_valueth.TabIndex = 15;
            this.numericUpDown_sep_valueth.Value = new decimal(new int[] {
            15,
            0,
            0,
            131072});
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(6, 243);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(94, 18);
            this.label9.TabIndex = 14;
            this.label9.Text = "sep_valueth";
            // 
            // numericUpDown_line_valueth
            // 
            this.numericUpDown_line_valueth.DecimalPlaces = 2;
            this.numericUpDown_line_valueth.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.numericUpDown_line_valueth.Location = new System.Drawing.Point(170, 205);
            this.numericUpDown_line_valueth.Maximum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDown_line_valueth.Name = "numericUpDown_line_valueth";
            this.numericUpDown_line_valueth.Size = new System.Drawing.Size(90, 25);
            this.numericUpDown_line_valueth.TabIndex = 13;
            this.numericUpDown_line_valueth.Value = new decimal(new int[] {
            25,
            0,
            0,
            131072});
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(6, 212);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(94, 18);
            this.label8.TabIndex = 12;
            this.label8.Text = "line_valueth";
            // 
            // numericUpDown_emphasis_cutoff
            // 
            this.numericUpDown_emphasis_cutoff.DecimalPlaces = 2;
            this.numericUpDown_emphasis_cutoff.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.numericUpDown_emphasis_cutoff.Location = new System.Drawing.Point(170, 174);
            this.numericUpDown_emphasis_cutoff.Maximum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDown_emphasis_cutoff.Name = "numericUpDown_emphasis_cutoff";
            this.numericUpDown_emphasis_cutoff.Size = new System.Drawing.Size(90, 25);
            this.numericUpDown_emphasis_cutoff.TabIndex = 11;
            this.numericUpDown_emphasis_cutoff.Value = new decimal(new int[] {
            5,
            0,
            0,
            65536});
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(6, 181);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(125, 18);
            this.label7.TabIndex = 10;
            this.label7.Text = "emphasis_cutoff";
            // 
            // numericUpDown_space_cutoff
            // 
            this.numericUpDown_space_cutoff.DecimalPlaces = 2;
            this.numericUpDown_space_cutoff.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.numericUpDown_space_cutoff.Location = new System.Drawing.Point(170, 143);
            this.numericUpDown_space_cutoff.Maximum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDown_space_cutoff.Name = "numericUpDown_space_cutoff";
            this.numericUpDown_space_cutoff.Size = new System.Drawing.Size(90, 25);
            this.numericUpDown_space_cutoff.TabIndex = 9;
            this.numericUpDown_space_cutoff.Value = new decimal(new int[] {
            5,
            0,
            0,
            65536});
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(6, 150);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(100, 18);
            this.label6.TabIndex = 8;
            this.label6.Text = "space_cutoff";
            // 
            // numericUpDown_rubybase_cutoff
            // 
            this.numericUpDown_rubybase_cutoff.DecimalPlaces = 2;
            this.numericUpDown_rubybase_cutoff.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.numericUpDown_rubybase_cutoff.Location = new System.Drawing.Point(170, 112);
            this.numericUpDown_rubybase_cutoff.Maximum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDown_rubybase_cutoff.Name = "numericUpDown_rubybase_cutoff";
            this.numericUpDown_rubybase_cutoff.Size = new System.Drawing.Size(90, 25);
            this.numericUpDown_rubybase_cutoff.TabIndex = 7;
            this.numericUpDown_rubybase_cutoff.Value = new decimal(new int[] {
            75,
            0,
            0,
            131072});
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(6, 119);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(124, 18);
            this.label5.TabIndex = 6;
            this.label5.Text = "rubybase_cutoff";
            // 
            // numericUpDown_ruby_cutoff
            // 
            this.numericUpDown_ruby_cutoff.DecimalPlaces = 2;
            this.numericUpDown_ruby_cutoff.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.numericUpDown_ruby_cutoff.Location = new System.Drawing.Point(170, 81);
            this.numericUpDown_ruby_cutoff.Maximum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDown_ruby_cutoff.Name = "numericUpDown_ruby_cutoff";
            this.numericUpDown_ruby_cutoff.Size = new System.Drawing.Size(90, 25);
            this.numericUpDown_ruby_cutoff.TabIndex = 5;
            this.numericUpDown_ruby_cutoff.Value = new decimal(new int[] {
            25,
            0,
            0,
            131072});
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(6, 88);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(89, 18);
            this.label4.TabIndex = 4;
            this.label4.Text = "ruby_cutoff";
            // 
            // numericUpDown_blank_cutoff
            // 
            this.numericUpDown_blank_cutoff.Location = new System.Drawing.Point(170, 50);
            this.numericUpDown_blank_cutoff.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.numericUpDown_blank_cutoff.Name = "numericUpDown_blank_cutoff";
            this.numericUpDown_blank_cutoff.Size = new System.Drawing.Size(90, 25);
            this.numericUpDown_blank_cutoff.TabIndex = 3;
            this.numericUpDown_blank_cutoff.Value = new decimal(new int[] {
            15,
            0,
            0,
            0});
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(6, 57);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(95, 18);
            this.label3.TabIndex = 2;
            this.label3.Text = "blank_cutoff";
            // 
            // numericUpDown_detect_cut_off
            // 
            this.numericUpDown_detect_cut_off.DecimalPlaces = 2;
            this.numericUpDown_detect_cut_off.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.numericUpDown_detect_cut_off.Location = new System.Drawing.Point(170, 19);
            this.numericUpDown_detect_cut_off.Maximum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDown_detect_cut_off.Name = "numericUpDown_detect_cut_off";
            this.numericUpDown_detect_cut_off.Size = new System.Drawing.Size(90, 25);
            this.numericUpDown_detect_cut_off.TabIndex = 1;
            this.numericUpDown_detect_cut_off.Value = new decimal(new int[] {
            35,
            0,
            0,
            131072});
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 26);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(109, 18);
            this.label2.TabIndex = 0;
            this.label2.Text = "detect_cut_off";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.label14);
            this.groupBox3.Controls.Add(this.numericUpDown_sleep_wait);
            this.groupBox3.Controls.Add(this.label13);
            this.groupBox3.Controls.Add(this.numericUpDown_resize);
            this.groupBox3.Controls.Add(this.label12);
            this.groupBox3.Location = new System.Drawing.Point(334, 12);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(292, 106);
            this.groupBox3.TabIndex = 3;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Process parameter";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(202, 57);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(34, 18);
            this.label14.TabIndex = 6;
            this.label14.Text = "sec";
            // 
            // numericUpDown_sleep_wait
            // 
            this.numericUpDown_sleep_wait.DecimalPlaces = 2;
            this.numericUpDown_sleep_wait.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.numericUpDown_sleep_wait.Location = new System.Drawing.Point(106, 57);
            this.numericUpDown_sleep_wait.Maximum = new decimal(new int[] {
            600,
            0,
            0,
            0});
            this.numericUpDown_sleep_wait.Name = "numericUpDown_sleep_wait";
            this.numericUpDown_sleep_wait.Size = new System.Drawing.Size(90, 25);
            this.numericUpDown_sleep_wait.TabIndex = 5;
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(6, 57);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(83, 18);
            this.label13.TabIndex = 4;
            this.label13.Text = "sleep_wait";
            // 
            // numericUpDown_resize
            // 
            this.numericUpDown_resize.DecimalPlaces = 2;
            this.numericUpDown_resize.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.numericUpDown_resize.Location = new System.Drawing.Point(106, 26);
            this.numericUpDown_resize.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.numericUpDown_resize.Name = "numericUpDown_resize";
            this.numericUpDown_resize.Size = new System.Drawing.Size(90, 25);
            this.numericUpDown_resize.TabIndex = 3;
            this.numericUpDown_resize.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(6, 26);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(52, 18);
            this.label12.TabIndex = 2;
            this.label12.Text = "resize";
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.checkBox_rawoutput);
            this.groupBox4.Controls.Add(this.label17);
            this.groupBox4.Controls.Add(this.textBox_rubyafter);
            this.groupBox4.Controls.Add(this.checkBox_output_ruby);
            this.groupBox4.Controls.Add(this.label_rubysample);
            this.groupBox4.Controls.Add(this.label16);
            this.groupBox4.Controls.Add(this.textBox_rubysepatator);
            this.groupBox4.Controls.Add(this.label15);
            this.groupBox4.Controls.Add(this.textBox_rubybefore);
            this.groupBox4.Controls.Add(this.radioButton_custum);
            this.groupBox4.Controls.Add(this.radioButton_aozora);
            this.groupBox4.Controls.Add(this.radioButton_no_ruby);
            this.groupBox4.Controls.Add(this.checkBox_output_text);
            this.groupBox4.Location = new System.Drawing.Point(311, 130);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(351, 309);
            this.groupBox4.TabIndex = 4;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Post process";
            // 
            // label_rubysample
            // 
            this.label_rubysample.AutoSize = true;
            this.label_rubysample.Location = new System.Drawing.Point(16, 281);
            this.label_rubysample.Name = "label_rubysample";
            this.label_rubysample.Size = new System.Drawing.Size(60, 18);
            this.label_rubysample.TabIndex = 10;
            this.label_rubysample.Text = "sample";
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(36, 237);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(43, 18);
            this.label17.TabIndex = 9;
            this.label17.Text = "after";
            // 
            // textBox_rubyafter
            // 
            this.textBox_rubyafter.Location = new System.Drawing.Point(121, 234);
            this.textBox_rubyafter.Name = "textBox_rubyafter";
            this.textBox_rubyafter.Size = new System.Drawing.Size(100, 25);
            this.textBox_rubyafter.TabIndex = 8;
            this.textBox_rubyafter.TextChanged += new System.EventHandler(this.textBox_rubyafter_TextChanged);
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(36, 179);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(79, 18);
            this.label16.TabIndex = 7;
            this.label16.Text = "separator";
            // 
            // textBox_rubysepatator
            // 
            this.textBox_rubysepatator.Location = new System.Drawing.Point(121, 176);
            this.textBox_rubysepatator.Name = "textBox_rubysepatator";
            this.textBox_rubysepatator.Size = new System.Drawing.Size(100, 25);
            this.textBox_rubysepatator.TabIndex = 6;
            this.textBox_rubysepatator.TextChanged += new System.EventHandler(this.textBox_rubysepatator_TextChanged);
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(36, 150);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(55, 18);
            this.label15.TabIndex = 5;
            this.label15.Text = "before";
            // 
            // textBox_rubybefore
            // 
            this.textBox_rubybefore.Location = new System.Drawing.Point(121, 147);
            this.textBox_rubybefore.Name = "textBox_rubybefore";
            this.textBox_rubybefore.Size = new System.Drawing.Size(100, 25);
            this.textBox_rubybefore.TabIndex = 4;
            this.textBox_rubybefore.TextChanged += new System.EventHandler(this.textBox_rubybefore_TextChanged);
            // 
            // radioButton_custum
            // 
            this.radioButton_custum.AutoSize = true;
            this.radioButton_custum.Location = new System.Drawing.Point(6, 119);
            this.radioButton_custum.Name = "radioButton_custum";
            this.radioButton_custum.Size = new System.Drawing.Size(143, 22);
            this.radioButton_custum.TabIndex = 3;
            this.radioButton_custum.TabStop = true;
            this.radioButton_custum.Text = "Custum format";
            this.radioButton_custum.UseVisualStyleBackColor = true;
            this.radioButton_custum.CheckedChanged += new System.EventHandler(this.radioButton_custum_CheckedChanged);
            // 
            // radioButton_aozora
            // 
            this.radioButton_aozora.AutoSize = true;
            this.radioButton_aozora.Location = new System.Drawing.Point(6, 91);
            this.radioButton_aozora.Name = "radioButton_aozora";
            this.radioButton_aozora.Size = new System.Drawing.Size(138, 22);
            this.radioButton_aozora.TabIndex = 2;
            this.radioButton_aozora.TabStop = true;
            this.radioButton_aozora.Text = "Aozora format";
            this.radioButton_aozora.UseVisualStyleBackColor = true;
            this.radioButton_aozora.CheckedChanged += new System.EventHandler(this.radioButton_aozora_CheckedChanged);
            // 
            // radioButton_no_ruby
            // 
            this.radioButton_no_ruby.AutoSize = true;
            this.radioButton_no_ruby.Location = new System.Drawing.Point(6, 63);
            this.radioButton_no_ruby.Name = "radioButton_no_ruby";
            this.radioButton_no_ruby.Size = new System.Drawing.Size(131, 22);
            this.radioButton_no_ruby.TabIndex = 1;
            this.radioButton_no_ruby.TabStop = true;
            this.radioButton_no_ruby.Text = "Remove ruby";
            this.radioButton_no_ruby.UseVisualStyleBackColor = true;
            this.radioButton_no_ruby.CheckedChanged += new System.EventHandler(this.radioButton_no_ruby_CheckedChanged);
            // 
            // checkBox_output_text
            // 
            this.checkBox_output_text.AutoSize = true;
            this.checkBox_output_text.Location = new System.Drawing.Point(148, 26);
            this.checkBox_output_text.Name = "checkBox_output_text";
            this.checkBox_output_text.Size = new System.Drawing.Size(142, 22);
            this.checkBox_output_text.TabIndex = 0;
            this.checkBox_output_text.Text = "Output as text";
            this.checkBox_output_text.UseVisualStyleBackColor = true;
            // 
            // checkBox_output_ruby
            // 
            this.checkBox_output_ruby.AutoSize = true;
            this.checkBox_output_ruby.Location = new System.Drawing.Point(39, 207);
            this.checkBox_output_ruby.Name = "checkBox_output_ruby";
            this.checkBox_output_ruby.Size = new System.Drawing.Size(120, 22);
            this.checkBox_output_ruby.TabIndex = 11;
            this.checkBox_output_ruby.Text = "output ruby";
            this.checkBox_output_ruby.UseVisualStyleBackColor = true;
            this.checkBox_output_ruby.CheckedChanged += new System.EventHandler(this.checkBox_output_ruby_CheckedChanged);
            // 
            // checkBox_rawoutput
            // 
            this.checkBox_rawoutput.AutoSize = true;
            this.checkBox_rawoutput.Location = new System.Drawing.Point(6, 26);
            this.checkBox_rawoutput.Name = "checkBox_rawoutput";
            this.checkBox_rawoutput.Size = new System.Drawing.Size(119, 22);
            this.checkBox_rawoutput.TabIndex = 12;
            this.checkBox_rawoutput.Text = "Raw output";
            this.checkBox_rawoutput.UseVisualStyleBackColor = true;
            this.checkBox_rawoutput.CheckedChanged += new System.EventHandler(this.checkBox_rawoutput_CheckedChanged);
            // 
            // Form3
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 18F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(687, 524);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.button1);
            this.Name = "Form3";
            this.Text = "Config";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_GPUid)).EndInit();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_allowwidth_next)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_sep_valueth2)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_sep_valueth)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_line_valueth)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_emphasis_cutoff)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_space_cutoff)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_rubybase_cutoff)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_ruby_cutoff)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_blank_cutoff)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_detect_cut_off)).EndInit();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_sleep_wait)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_resize)).EndInit();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.CheckBox checkBox_DML;
        private System.Windows.Forms.CheckBox checkBox_CUDA;
        private System.Windows.Forms.CheckBox checkBox_TensorRT;
        private System.Windows.Forms.CheckBox checkBox_OpneVINO;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown numericUpDown_GPUid;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.NumericUpDown numericUpDown_detect_cut_off;
        private System.Windows.Forms.NumericUpDown numericUpDown_blank_cutoff;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.NumericUpDown numericUpDown_ruby_cutoff;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.NumericUpDown numericUpDown_rubybase_cutoff;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.NumericUpDown numericUpDown_space_cutoff;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.NumericUpDown numericUpDown_emphasis_cutoff;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.NumericUpDown numericUpDown_line_valueth;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.NumericUpDown numericUpDown_sep_valueth;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.NumericUpDown numericUpDown_sep_valueth2;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.NumericUpDown numericUpDown_allowwidth_next;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.NumericUpDown numericUpDown_resize;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.NumericUpDown numericUpDown_sleep_wait;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.CheckBox checkBox_output_text;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.TextBox textBox_rubybefore;
        private System.Windows.Forms.RadioButton radioButton_custum;
        private System.Windows.Forms.RadioButton radioButton_aozora;
        private System.Windows.Forms.RadioButton radioButton_no_ruby;
        private System.Windows.Forms.Label label_rubysample;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.TextBox textBox_rubyafter;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.TextBox textBox_rubysepatator;
        private System.Windows.Forms.CheckBox checkBox_output_ruby;
        private System.Windows.Forms.CheckBox checkBox_rawoutput;
    }
}