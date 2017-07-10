using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows.Forms;
using System.Diagnostics;
using System.Threading;

namespace engine_dashboard
{
	class Builder
	{
		public enum Configuration
		{
			Debug,
			Development,
			Release
		}

		public enum Platform
		{
			Win32,
			Win64,
			iOS,
			Android,
			OSX,
			Linux,
			Playstation4,
			XboxOne
		}

		public class PlatformNames
		{
			public static string Win32 = "win32";
			public static string Win64 = "win64";
			public static string iOS = "ios";
			public static string Android = "android";
			public static string OSX = "osx";
			public static string Linux = "linux";
			public static string Playstation4 = "ps4";
			public static string XboxOne = "xbone";
		}

		public bool b3rdparty;
		public bool bEngine;
		public bool bClean;
		public bool bCompile;
		public Builder.Configuration configuration;
		public Builder.Platform platform;
		public bool bBuildCancelled;
		public MainForm mainForm;

		public Builder()
		{
			bBuildCancelled = false;
		}

		public void build()
		{
			if (bClean)
			{
				buildClean();
			}

			if (b3rdparty)
			{
				build3rdparty();
			}

			if (bEngine)
			{
				buildEngine();
			}

			if (bCompile)
			{
				buildCompile();
			}
		}

		public string getBuildDir()
		{
			switch(platform)
			{
				case Platform.Win32:
				return "build_" + PlatformNames.Win32;
				case Platform.Win64:
				return "build_" + PlatformNames.Win64;
				case Platform.iOS:
				return "build_" + PlatformNames.iOS;
				case Platform.Android:
				return "build_" + PlatformNames.Android;
				case Platform.OSX:
				return "build_" + PlatformNames.OSX;
				case Platform.Linux:
				return "build_" + PlatformNames.Linux;
				case Platform.Playstation4:
				return "build_" + PlatformNames.Playstation4;
				case Platform.XboxOne:
				return "build_" + PlatformNames.XboxOne;
			}

			return "";
		}

		private bool CopyFile(string from, string to)
		{
			String destination = Path.GetDirectoryName(to);
	
			try
			{
				if(!Directory.Exists(destination))
					Directory.CreateDirectory(destination);
				File.Copy(from, to, true);
			}

			catch(Exception)
			{
				return false;
			}

			return true;
		}

		private static void DirectoryCopy(string sourceDirName, string destDirName, bool copySubDirs)
		{
			// Get the subdirectories for the specified directory.
			DirectoryInfo dir = new DirectoryInfo(sourceDirName);
			DirectoryInfo[] dirs = dir.GetDirectories();

			if(!dir.Exists)
			{
				throw new DirectoryNotFoundException(
					"Source directory does not exist or could not be found: "
					+ sourceDirName);
			}

			// If the destination directory doesn't exist, create it. 
			if(!Directory.Exists(destDirName))
			{
				Directory.CreateDirectory(destDirName);
			}

			// Get the files in the directory and copy them to the new location.
			FileInfo[] files = dir.GetFiles();
			foreach(FileInfo file in files)
			{
				string temppath = Path.Combine(destDirName, file.Name);
				file.CopyTo(temppath, false);
			}

			// If copying subdirectories, copy them and their contents to new location. 
			if(copySubDirs)
			{
				foreach(DirectoryInfo subdir in dirs)
				{
					string temppath = Path.Combine(destDirName, subdir.Name);
					DirectoryCopy(subdir.FullName, temppath, copySubDirs);
				}
			}
		}

		private bool RunProcess(string filename, string args, string workingDir = "")
		{
			using(Process process = new Process())
			{
				process.StartInfo.FileName = filename;
				process.StartInfo.Arguments = args;
				process.StartInfo.UseShellExecute = false;
				process.StartInfo.CreateNoWindow = true;
				process.StartInfo.RedirectStandardOutput = true;
				process.StartInfo.RedirectStandardError = true;
				
				if(!String.IsNullOrEmpty(workingDir))
					process.StartInfo.WorkingDirectory = workingDir;

				process.OutputDataReceived += new DataReceivedEventHandler(OutputHandler);
				process.ErrorDataReceived += new DataReceivedEventHandler(ErrorHandler);

				try
				{
					process.Start();
				}
				catch
				{
					return false;
				}

				process.BeginOutputReadLine();
				process.BeginErrorReadLine();

				while(!process.HasExited)
				{
					if(bBuildCancelled)
					{
						process.CancelErrorRead();
						process.CancelOutputRead();
						process.Kill();
						break;
					}

					Application.DoEvents();
					Thread.Sleep(10);
				}

				process.WaitForExit();
				return process.ExitCode == 0;
			}
		}

		private void OutputHandler(object o, DataReceivedEventArgs args)
		{
			mainForm.logMessage(args.Data);
		}

		private void ErrorHandler(object o, DataReceivedEventArgs args)
		{
			mainForm.logMessage(args.Data);
		}

		public void buildClean()
		{
			if (Directory.Exists(getBuildDir()))
				Directory.Delete(getBuildDir(), true);
		
			if (Directory.Exists("bin"))
				Directory.Delete("bin", true);
		}

		public void runCMakeScript(string scriptDir, string buildDir, string args)
		{
			RunProcess("cmake", scriptDir + " " + args, buildDir);
		}

		public string getCMakeGenerator()
		{
			switch (platform)
			{
				case Platform.Win32:
					return "Visual Studio 10";
				case Platform.Win64:
					return "Visual Studio 10 Win64";
				case Platform.iOS:
					return "Xcode";
				case Platform.Android:
					return "";
				case Platform.OSX:
					return "Xcode";
				case Platform.Linux:
					return "Unix Makefiles";
				case Platform.Playstation4:
					return "";
				case Platform.XboxOne:
					return "";
			}

			return "";
		}

		public void build3rdparty()
		{
			DirectoryCopy("./code/3rdparty/binaries", "bin", true);
			runCMakeScript("code/3rdparty", getBuildDir(), "-G\"" + getCMakeGenerator() + "\" -DCMAKE_INSTALL_PREFIX:PATH=\"" + getBuildDir() + "\"");
		}

		public void buildEngine()
		{
			runCMakeScript("code", getBuildDir(), "-G\"" + getCMakeGenerator() + "\" -DCMAKE_INSTALL_PREFIX:PATH=\"" + getBuildDir() + "\"");
		}

		public void buildCompile()
		{

		}
	}
}
