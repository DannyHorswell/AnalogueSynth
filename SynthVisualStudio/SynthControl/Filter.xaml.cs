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

namespace SynthControl
{
    /// <summary>
    /// Interaction logic for Filter.xaml
    /// </summary>
    public partial class Filter : UserControl
    {
        public Filter()
        {
            InitializeComponent();
        }

        public void Initalise()
        {
            // Set the change events
            SetEventsAndInitialValues(root);
        }

        public int WGID { get; set; }

        void SetEventsAndInitialValues(Visual parentVisual)
        {
            for (int i = 0; i < VisualTreeHelper.GetChildrenCount(parentVisual); i++)
            {
                Visual childVisual = (Visual)VisualTreeHelper.GetChild(parentVisual, i);

                if (childVisual is Slider)
                {
                    ((Slider)childVisual).ValueChanged += new RoutedPropertyChangedEventHandler<double>(Slider_ValueChanged);
                    SetSliderUpperAndLowerLevels((Slider) childVisual);
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
                case "Q":
                    TheSlider.Minimum = 0.1;
                    TheSlider.Maximum = 100.0;
                    TheSlider.Value = 1.0;
                    break;

                case "RelativeFrequency":
                    TheSlider.Minimum = 0.0;
                    TheSlider.Maximum = 10.0;
                    break;

                // db Gain
                case "DBGain":
                    TheSlider.Minimum = -18.0;
                    TheSlider.Maximum = 18.0;
                    break;

                case "VelocityFactor":
                    TheSlider.Minimum = -36.0;
                    TheSlider.Maximum = 36.0;
                    break;

                default:
                    MessageBox.Show("Bad slider name " + TheSlider.Name);
                    break;
            }
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

            string command = string.Format("Patch:WG[" + WGID + "].Filter." + theName + " = {0}", value);

            MainWindow.MainWindowSingleTon.SendSShCommand(command);
        }

        private void ListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var theList = sender as ListBox;

            string theName = theList.Name;

            string command = string.Format("Patch:WG[" + WGID + "].Filter." + theName + " = {0}", ((System.Windows.Controls.ContentControl)(theList.SelectedItem)).Content);

            MainWindow.MainWindowSingleTon.SendSShCommand(command);
        }
    }
}
