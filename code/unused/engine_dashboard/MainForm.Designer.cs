namespace engine_dashboard
{
	partial class MainForm
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose( bool disposing )
		{
			if( disposing && ( components != null ) )
			{
				components.Dispose();
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
			this.panel1 = new System.Windows.Forms.Panel();
			this.label6 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.label5 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.comboBoxConfig = new System.Windows.Forms.ComboBox();
			this.comboBoxPlatform = new System.Windows.Forms.ComboBox();
			this.checkBoxCompile = new System.Windows.Forms.CheckBox();
			this.checkBoxCleanBuild = new System.Windows.Forms.CheckBox();
			this.checkBoxEngine = new System.Windows.Forms.CheckBox();
			this.checkBox3rdparty = new System.Windows.Forms.CheckBox();
			this.buttonStartBuild = new System.Windows.Forms.Button();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.label4 = new System.Windows.Forms.Label();
			this.comboBoxProjects = new System.Windows.Forms.ComboBox();
			this.buttonEditor = new System.Windows.Forms.Button();
			this.buttonProjectManagement = new System.Windows.Forms.Button();
			this.buttonLaunch = new System.Windows.Forms.Button();
			this.listViewLog = new System.Windows.Forms.ListView();
			this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.panel1.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.SuspendLayout();
			// 
			// panel1
			// 
			this.panel1.BackColor = System.Drawing.Color.DeepSkyBlue;
			this.panel1.Controls.Add(this.label6);
			this.panel1.Controls.Add(this.label2);
			this.panel1.Controls.Add(this.label1);
			this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
			this.panel1.Location = new System.Drawing.Point(0, 0);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(629, 63);
			this.panel1.TabIndex = 0;
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Font = new System.Drawing.Font("Tahoma", 10F, System.Drawing.FontStyle.Bold);
			this.label6.ForeColor = System.Drawing.Color.White;
			this.label6.Location = new System.Drawing.Point(542, 38);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(73, 17);
			this.label6.TabIndex = 0;
			this.label6.Text = "ver. 1.0.0";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Font = new System.Drawing.Font("Tahoma", 12F, System.Drawing.FontStyle.Bold);
			this.label2.ForeColor = System.Drawing.Color.White;
			this.label2.Location = new System.Drawing.Point(100, 38);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(96, 19);
			this.label2.TabIndex = 0;
			this.label2.Text = "Dashboard";
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Font = new System.Drawing.Font("Tahoma", 18F, System.Drawing.FontStyle.Bold);
			this.label1.ForeColor = System.Drawing.Color.White;
			this.label1.Location = new System.Drawing.Point(38, 9);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(169, 29);
			this.label1.TabIndex = 0;
			this.label1.Text = "Nytro Engine";
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.label5);
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.comboBoxConfig);
			this.groupBox1.Controls.Add(this.comboBoxPlatform);
			this.groupBox1.Controls.Add(this.checkBoxCompile);
			this.groupBox1.Controls.Add(this.checkBoxCleanBuild);
			this.groupBox1.Controls.Add(this.checkBoxEngine);
			this.groupBox1.Controls.Add(this.checkBox3rdparty);
			this.groupBox1.Controls.Add(this.buttonStartBuild);
			this.groupBox1.Location = new System.Drawing.Point(387, 69);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(241, 166);
			this.groupBox1.TabIndex = 1;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Code Build";
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(121, 33);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(72, 13);
			this.label5.TabIndex = 3;
			this.label5.Text = "Configuration:";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(121, 77);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(48, 13);
			this.label3.TabIndex = 3;
			this.label3.Text = "Platform:";
			// 
			// comboBoxConfig
			// 
			this.comboBoxConfig.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboBoxConfig.FormattingEnabled = true;
			this.comboBoxConfig.Items.AddRange(new object[] {
            "Debug",
            "Development",
            "Release"});
			this.comboBoxConfig.Location = new System.Drawing.Point(124, 50);
			this.comboBoxConfig.Name = "comboBoxConfig";
			this.comboBoxConfig.Size = new System.Drawing.Size(104, 21);
			this.comboBoxConfig.TabIndex = 2;
			// 
			// comboBoxPlatform
			// 
			this.comboBoxPlatform.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboBoxPlatform.FormattingEnabled = true;
			this.comboBoxPlatform.Items.AddRange(new object[] {
            "Win32",
            "Win64",
            "iOS",
            "Android",
            "OSX",
            "Linux",
            "Playstation4",
            "XboxOne"});
			this.comboBoxPlatform.Location = new System.Drawing.Point(124, 93);
			this.comboBoxPlatform.Name = "comboBoxPlatform";
			this.comboBoxPlatform.Size = new System.Drawing.Size(104, 21);
			this.comboBoxPlatform.TabIndex = 2;
			// 
			// checkBoxCompile
			// 
			this.checkBoxCompile.AutoSize = true;
			this.checkBoxCompile.Location = new System.Drawing.Point(17, 98);
			this.checkBoxCompile.Name = "checkBoxCompile";
			this.checkBoxCompile.Size = new System.Drawing.Size(63, 17);
			this.checkBoxCompile.TabIndex = 1;
			this.checkBoxCompile.Text = "Compile";
			this.checkBoxCompile.UseVisualStyleBackColor = true;
			// 
			// checkBoxCleanBuild
			// 
			this.checkBoxCleanBuild.AutoSize = true;
			this.checkBoxCleanBuild.Location = new System.Drawing.Point(17, 75);
			this.checkBoxCleanBuild.Name = "checkBoxCleanBuild";
			this.checkBoxCleanBuild.Size = new System.Drawing.Size(78, 17);
			this.checkBoxCleanBuild.TabIndex = 1;
			this.checkBoxCleanBuild.Text = "Clean build";
			this.checkBoxCleanBuild.UseVisualStyleBackColor = true;
			// 
			// checkBoxEngine
			// 
			this.checkBoxEngine.AutoSize = true;
			this.checkBoxEngine.Location = new System.Drawing.Point(17, 52);
			this.checkBoxEngine.Name = "checkBoxEngine";
			this.checkBoxEngine.Size = new System.Drawing.Size(59, 17);
			this.checkBoxEngine.TabIndex = 1;
			this.checkBoxEngine.Text = "Engine";
			this.checkBoxEngine.UseVisualStyleBackColor = true;
			// 
			// checkBox3rdparty
			// 
			this.checkBox3rdparty.AutoSize = true;
			this.checkBox3rdparty.Location = new System.Drawing.Point(17, 29);
			this.checkBox3rdparty.Name = "checkBox3rdparty";
			this.checkBox3rdparty.Size = new System.Drawing.Size(64, 17);
			this.checkBox3rdparty.TabIndex = 1;
			this.checkBox3rdparty.Text = "3rdparty";
			this.checkBox3rdparty.UseVisualStyleBackColor = true;
			// 
			// buttonStartBuild
			// 
			this.buttonStartBuild.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.buttonStartBuild.Location = new System.Drawing.Point(124, 123);
			this.buttonStartBuild.Name = "buttonStartBuild";
			this.buttonStartBuild.Size = new System.Drawing.Size(104, 26);
			this.buttonStartBuild.TabIndex = 0;
			this.buttonStartBuild.Text = "Build";
			this.buttonStartBuild.UseVisualStyleBackColor = true;
			this.buttonStartBuild.Click += new System.EventHandler(this.buttonStartBuild_Click);
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.label4);
			this.groupBox2.Controls.Add(this.comboBoxProjects);
			this.groupBox2.Controls.Add(this.buttonEditor);
			this.groupBox2.Controls.Add(this.buttonProjectManagement);
			this.groupBox2.Controls.Add(this.buttonLaunch);
			this.groupBox2.Location = new System.Drawing.Point(0, 69);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(381, 166);
			this.groupBox2.TabIndex = 3;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "Project";
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(13, 29);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(79, 13);
			this.label4.TabIndex = 3;
			this.label4.Text = "Current project:";
			// 
			// comboBoxProjects
			// 
			this.comboBoxProjects.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboBoxProjects.Font = new System.Drawing.Font("Microsoft Sans Serif", 14F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.comboBoxProjects.FormattingEnabled = true;
			this.comboBoxProjects.Items.AddRange(new object[] {
            "Eternal"});
			this.comboBoxProjects.Location = new System.Drawing.Point(16, 50);
			this.comboBoxProjects.Name = "comboBoxProjects";
			this.comboBoxProjects.Size = new System.Drawing.Size(207, 32);
			this.comboBoxProjects.TabIndex = 2;
			this.comboBoxProjects.SelectedIndexChanged += new System.EventHandler(this.comboBoxProjects_SelectedIndexChanged);
			// 
			// buttonEditor
			// 
			this.buttonEditor.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.buttonEditor.Location = new System.Drawing.Point(229, 88);
			this.buttonEditor.Name = "buttonEditor";
			this.buttonEditor.Size = new System.Drawing.Size(136, 29);
			this.buttonEditor.TabIndex = 0;
			this.buttonEditor.Text = "Editor...";
			this.buttonEditor.UseVisualStyleBackColor = true;
			this.buttonEditor.Click += new System.EventHandler(this.buttonEditor_Click);
			// 
			// buttonProjectManagement
			// 
			this.buttonProjectManagement.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.buttonProjectManagement.Location = new System.Drawing.Point(16, 123);
			this.buttonProjectManagement.Name = "buttonProjectManagement";
			this.buttonProjectManagement.Size = new System.Drawing.Size(207, 29);
			this.buttonProjectManagement.TabIndex = 0;
			this.buttonProjectManagement.Text = "Project Management...";
			this.buttonProjectManagement.UseVisualStyleBackColor = true;
			this.buttonProjectManagement.Click += new System.EventHandler(this.buttonProjectManagement_Click);
			// 
			// buttonLaunch
			// 
			this.buttonLaunch.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.buttonLaunch.Location = new System.Drawing.Point(16, 88);
			this.buttonLaunch.Name = "buttonLaunch";
			this.buttonLaunch.Size = new System.Drawing.Size(207, 29);
			this.buttonLaunch.TabIndex = 0;
			this.buttonLaunch.Text = "Launch";
			this.buttonLaunch.UseVisualStyleBackColor = true;
			this.buttonLaunch.Click += new System.EventHandler(this.buttonLaunch_Click);
			// 
			// listViewLog
			// 
			this.listViewLog.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.listViewLog.BackColor = System.Drawing.Color.Black;
			this.listViewLog.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1});
			this.listViewLog.ForeColor = System.Drawing.Color.White;
			this.listViewLog.Location = new System.Drawing.Point(0, 241);
			this.listViewLog.Name = "listViewLog";
			this.listViewLog.Size = new System.Drawing.Size(628, 189);
			this.listViewLog.TabIndex = 5;
			this.listViewLog.UseCompatibleStateImageBehavior = false;
			this.listViewLog.View = System.Windows.Forms.View.List;
			// 
			// columnHeader1
			// 
			this.columnHeader1.Text = "Message";
			this.columnHeader1.Width = 595;
			// 
			// MainForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(629, 431);
			this.Controls.Add(this.listViewLog);
			this.Controls.Add(this.groupBox2);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.panel1);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "MainForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Nytro Engine Dashboard";
			this.Load += new System.EventHandler(this.MainForm_Load);
			this.panel1.ResumeLayout(false);
			this.panel1.PerformLayout();
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.groupBox2.ResumeLayout(false);
			this.groupBox2.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.ComboBox comboBoxPlatform;
		private System.Windows.Forms.CheckBox checkBoxEngine;
		private System.Windows.Forms.CheckBox checkBox3rdparty;
		private System.Windows.Forms.Button buttonStartBuild;
		private System.Windows.Forms.CheckBox checkBoxCompile;
		private System.Windows.Forms.CheckBox checkBoxCleanBuild;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.GroupBox groupBox2;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.ComboBox comboBoxProjects;
		private System.Windows.Forms.Button buttonLaunch;
		private System.Windows.Forms.Button buttonProjectManagement;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.ComboBox comboBoxConfig;
		private System.Windows.Forms.Button buttonEditor;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.ListView listViewLog;
		private System.Windows.Forms.ColumnHeader columnHeader1;
	}
}

