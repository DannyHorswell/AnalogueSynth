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
using System.Windows.Shapes;
using System.ComponentModel;

namespace SynthControl
{
    /// <summary>
    /// Interaction logic for Reverb.xaml
    /// </summary>
    public partial class Reverb : Window
    {
        public Reverb()
        {
            InitializeComponent();
            // Set the change events
            SetEventsAndInitialValues(root);
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

                SetEventsAndInitialValues(childVisual);
            }
        }

        void SetSliderUpperAndLowerLevels(Slider TheSlider)
        {
            switch (TheSlider.Name)
            {
                // + only Unity gain levels
                case "Feedback":
                case "Level":
                    TheSlider.Minimum = 0.0;
                    TheSlider.Maximum = 1.0;
                    break;

                // Time levels
                case "SampleLength":
                    TheSlider.Maximum = 20000.0;
                    TheSlider.Minimum = 0.0;
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

            float value = (float)e.NewValue;

            string command = string.Format("Patch:Reverb." + theName + " = {0}", value);

            MainWindow.MainWindowSingleTon.SendSShCommand(command);
        }

        private void CheckBox_Checked(object sender, RoutedEventArgs e)
        {
            var theCheck = sender as CheckBox;

            string theName = theCheck.Name;

            string command = string.Format("Patch:Reverb." + theName + " = {0}", theCheck.IsChecked.Value ? "1" : "0");

            MainWindow.MainWindowSingleTon.SendSShCommand(command);
        }
    }
}
