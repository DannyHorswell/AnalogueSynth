using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Threading;
using System.Threading;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.ComponentModel;
using System.IO;
using System.Net.Sockets;


namespace SynthControl
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        const string piIpAddress = "127.0.0.1";
        //const string piIpAddress = "192.168.10.243";
        //const string piIpAddress = "192.168.10.207";
        const int TCP_PORT = 1234;

        public static MainWindow MainWindowSingleTon { get; set; }

        // TODO, make the number of windos dependant on setting and change static buttons to button list
        WG[] waveGenWindows = new WG[4];
        LFO[] lfoWindows = new LFO[2];
        Reverb reverbWindow = new Reverb();

        public MainWindow()
        {
            InitializeComponent();

            MainWindowSingleTon = this;

            keyboard.Initalise();

            // Create the wave generator windows
            for (int count = 0; count < waveGenWindows.Length; count++)
            {
                waveGenWindows[count] = new WG(count);
            }

            // Create the wave LFO windows
            for (int count = 0; count < lfoWindows.Length; count++)
            {
                lfoWindows[count] = new LFO(count);
            }

            //SetEventsAndInitialValues(root);
            SetEventsAndInitialValues(patchEdit);

            SetUpSSH();

            SendSShCommand("cd synth");
            SendSShCommand("./synth");
        }

        //static SshClient SSH_Client;
        //static ShellStream SSHStreamClient;
        //static StreamReader reader;
        //static StreamWriter writer;

        TcpClient theTCPClient = null;
        NetworkStream tcpStream = null;

        bool connected;

        void SetUpSSH()
        {
            /*SSH_Client = new SshClient(piIpAddress, 22, "pi", "raspberry");
            SSH_Client.Connect();

            SSHStreamClient = SSH_Client.CreateShellStream("xterm", 80, 24, 800, 600, 0, new KeyValuePair<Renci.SshNet.Common.TerminalModes, uint>(Renci.SshNet.Common.TerminalModes.ECHO, 53));

            reader = new StreamReader(SSHStreamClient);
            writer = new StreamWriter(SSHStreamClient);
            writer.AutoFlush = true;

            SSHStreamClient.DataReceived += new EventHandler<Renci.SshNet.Common.ShellDataEventArgs>(SSHStreamClient_DataReceived);

            connected = true;
            */

            theTCPClient = new TcpClient();
            theTCPClient.BeginConnect(piIpAddress, TCP_PORT, new AsyncCallback(SocketConnected), null); 
            //tcpStream = theTCPClient.GetStream();
        }

        public void SocketConnected(IAsyncResult res)
        {
            try
            {
                theTCPClient.EndConnect(res);
                tcpStream = theTCPClient.GetStream();

                connected = true;
            }
            catch (Exception Ex)
            {
                MessageBox.Show("Socket error " + Ex);
            }
        }

        public void SendSShCommand(string Command)
        {
            if (connected)
            {
                //writer.WriteLine(Command);
                
                var bytes = System.Text.Encoding.ASCII.GetBytes(Command + "\n");
                tcpStream.Write(bytes, 0, bytes.Length);
                LastCommand.Text = "LastCommand : " + Command;
            }
        }

        ~MainWindow()
        {
            
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            theTCPClient.Close();

            //SSH_Client.Disconnect();
        }

        string output = "";


        private void StartSynth_Click(object sender, RoutedEventArgs e)
        {
            //writer.WriteLine("cd synth");
            //writer.WriteLine("./synth");
        }


        private void WG1Button_Click(object sender, RoutedEventArgs e)
        {
            waveGenWindows[0].Show();
        }

        private void WG2Button_Click(object sender, RoutedEventArgs e)
        {
            waveGenWindows[1].Show();
        }

        private void WG3Button_Click(object sender, RoutedEventArgs e)
        {
            waveGenWindows[2].Show();
        }

        private void WG4Button_Click(object sender, RoutedEventArgs e)
        {
            waveGenWindows[3].Show();
        }

        private void LFO1Button_Click(object sender, RoutedEventArgs e)
        {
            lfoWindows[0].Show();
        }

        private void LFO2Button_Click(object sender, RoutedEventArgs e)
        {
            lfoWindows[1].Show();
        }

        private void ReverbButton_Click(object sender, RoutedEventArgs e)
        {
            reverbWindow.Show();
        }

        void SetEventsAndInitialValues(Visual parentVisual)
        {
            for (int i = 0; i < VisualTreeHelper.GetChildrenCount(parentVisual); i++)
            {
                Visual childVisual = (Visual)VisualTreeHelper.GetChild(parentVisual, i);

                if (childVisual is ListBox)
                {
                    ((ListBox)childVisual).SelectionChanged += new SelectionChangedEventHandler(ListBox_SelectionChanged);
                }

                SetEventsAndInitialValues(childVisual);
            }
        }

        private void ListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var theList = sender as ListBox;

            string theName = theList.Name;

            string command = string.Format("Patch:" + theName + " = {0}", ((System.Windows.Controls.ContentControl)(theList.SelectedItem)).Content);

            MainWindow.MainWindowSingleTon.SendSShCommand(command);
        }

        private void SendCommand_Click(object sender, RoutedEventArgs e)
        {
            MainWindow.MainWindowSingleTon.SendSShCommand("Patch:" + Command.Text);
        }

       
    }
}
