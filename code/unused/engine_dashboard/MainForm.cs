using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Xml;
using System.Windows.Forms;

namespace engine_dashboard
{
	public partial class MainForm : Form
	{
		static string settingsFilename = "engine_dashboard.xml";

		public MainForm()
		{
			InitializeComponent();
			comboBoxPlatform.SelectedIndex = 0;
			comboBoxConfig.SelectedIndex = 0;
			loadSettings();
		}

		private void saveSettings()
		{
			XmlWriter xml = XmlWriter.Create(settingsFilename);

			xml.WriteStartDocument();
			
			xml.WriteStartElement("settings");
				xml.WriteAttributeString("b3rdparty", checkBox3rdparty.Checked.ToString());
				xml.WriteAttributeString("bEngine", checkBoxEngine.Checked.ToString());
				xml.WriteAttributeString("bClean", checkBoxCleanBuild.Checked.ToString());
				xml.WriteAttributeString("bCompile", checkBoxCompile.Checked.ToString());
				xml.WriteAttributeString("currentProject", comboBoxProjects.Text.ToString());
			xml.WriteEndElement();
			
			xml.WriteEndDocument();
			xml.Close();
		}

		private void loadSettings()
		{
			XmlDocument doc = new XmlDocument();

			try 
			{
				doc.Load(settingsFilename);
			}

			catch (Exception e)
			{
				return;
			}

			XmlNode node = doc.DocumentElement.SelectSingleNode("/settings");

			checkBox3rdparty.Checked = node.Attributes["b3rdparty"].Value == "True";
			checkBoxEngine.Checked = node.Attributes["bEngine"].Value == "True";
			checkBoxCleanBuild.Checked = node.Attributes["bClean"].Value == "True";
			checkBoxCompile.Checked = node.Attributes["bCompile"].Value == "True";
		}

		private void buttonStartBuild_Click( object sender, EventArgs e )
		{
			saveSettings();
			Builder builder = new Builder();

			builder.b3rdparty = checkBox3rdparty.Checked;
			builder.bEngine = checkBoxEngine.Checked;
			builder.bClean = checkBoxCleanBuild.Checked;
			builder.bCompile = checkBoxCompile.Checked;
			builder.configuration = (Builder.Configuration)comboBoxConfig.SelectedIndex;
			builder.platform = (Builder.Platform)comboBoxPlatform.SelectedIndex;
			builder.mainForm = this;
			builder.build();
		}

		public void logMessage(string msg)
		{
			ListViewItem item = listViewLog.Items.Add(msg);
			//item.ForeColor = color;
		}

		private void buttonLaunch_Click( object sender, EventArgs e )
		{
			saveSettings();

		}

		private void buttonEditor_Click( object sender, EventArgs e )
		{
			saveSettings();

		}

		private void buttonProjectManagement_Click( object sender, EventArgs e )
		{
			saveSettings();

		}

		private void comboBoxProjects_SelectedIndexChanged( object sender, EventArgs e )
		{
			saveSettings();

		}

		private void MainForm_Load( object sender, EventArgs e )
		{

		}
	}
}
