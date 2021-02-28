using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.ComponentModel;

namespace SynthControl
{

    /// <summary>
    /// Interaction logic for WG.xaml
    /// </summary>
    public partial class WG : Window
    {
        int _wgID;

        public WG(int WGID)
        {
            _wgID = WGID;

            InitializeComponent();

            // Set wave generator IDs
            TDP.WGID = _wgID;
            TDP.Initalise("TDP");

            TDA.WGID = _wgID;
            TDA.Initalise("TDA");

            TheFilter.WGID = _wgID;
            TheFilter.Initalise();

            // Set the change events
            SetEventsAndInitialValues(root);

            this.Title = "WG " + _wgID;
        }

        void SetEventsAndInitialValues(Visual parentVisual)
        {
            for (int i = 0; i < VisualTreeHelper.GetChildrenCount(parentVisual); i++)
            {
                Visual childVisual = (Visual)VisualTreeHelper.GetChild(parentVisual, i);

                if (childVisual is Slider)
                {
                    ((Slider)childVisual).ValueChanged += new RoutedPropertyChangedEventHandler<double>(Slider_ValueChanged);
                    SetSliderUpperAndLowerLevels((Slider)childVisual);
                }

                if (childVisual is CheckBox)
                {
                    ((CheckBox)childVisual).Checked += new RoutedEventHandler(CheckBox_Checked);
                }

                if (childVisual is ListBox)
                {
                    ((ListBox)childVisual).SelectionChanged += new SelectionChangedEventHandler(ListBox_SelectionChanged);
                }

                SetEventsAndInitialValues(childVisual);
            }
        }

        void SetSliderUpperAndLowerLevels(Slider TheSlider)
        {
            switch (TheSlider.Name)
            {
                // +- Semitone levels
                case "KeyOffestSemitones":
                case "PitchBendAmount":
                    TheSlider.Minimum = -12.0;
                    TheSlider.Maximum = 12.0;
                    break;

                // + only Semitone gain levels
                case "FreqLFOLevel":
                    TheSlider.Minimum = 0.0;
                    TheSlider.Maximum = 12.0;
                    break;

                // + only Unity gain levels
                case "PWMLFOLevel":
                    TheSlider.Minimum = 0.0;
                    TheSlider.Maximum = 1.0;
                    break;

                // Time levels
                case "FreqLFODelay":
                    TheSlider.Minimum = 0.0;
                    TheSlider.Maximum = 30.0;
                    break;

                // Velocity volumne
                case "VelocityVolumeAdjust":
                    TheSlider.Minimum = -1.0;
                    TheSlider.Maximum = 1.0;
                    break;

                default:
                    MessageBox.Show("Bad slider name " + TheSlider.Name);
                    break;
            }
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            e.Cancel = true;
            this.Visibility = System.Windows.Visibility.Hidden;
        }

        /// <summary>
        /// All slider events go here, we work out which from their names
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            var theSlider = sender as Slider;

            string theName = theSlider.Name;

            float value = (float) e.NewValue;

            string command = string.Format("Patch:WG[" + _wgID + "]." + theName + " = {0}", value);

            MainWindow.MainWindowSingleTon.SendSShCommand(command);
        }

        private void CheckBox_Checked(object sender, RoutedEventArgs e)
        {
            var theCheck = sender as CheckBox;

            string theName = theCheck.Name;

            string command = string.Format("Patch:WG[" + _wgID + "]." + theName + " = {0}", theCheck.IsChecked.Value ? "1" : "0");

            MainWindow.MainWindowSingleTon.SendSShCommand(command);
        }

        private void ListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var theList = sender as ListBox;

            string theName = theList.Name;

            string command = string.Format("Patch:WG[" + _wgID + "]." + theName + " = {0}", ((System.Windows.Controls.ContentControl)(theList.SelectedItem)).Content);

            MainWindow.MainWindowSingleTon.SendSShCommand(command);
        }
    }
}
