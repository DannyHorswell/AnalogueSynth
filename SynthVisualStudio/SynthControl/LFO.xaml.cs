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
    /// Interaction logic for LFO.xaml
    /// </summary>
    public partial class LFO : Window
    {
        int _LFOID;

        public LFO(int LFOID)
        {
            _LFOID = LFOID;
            InitializeComponent();

            // Set the change events
            SetEventsAndInitialValues(root);

            this.Title = "LFO " + _LFOID;
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
                case "Frequency":
                    TheSlider.Minimum = 0.0F;
                    TheSlider.Maximum = 10.0;
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

            string command = string.Format("Patch:LFO[" + _LFOID + "]." + theName + " = {0}", value);

            MainWindow.MainWindowSingleTon.SendSShCommand(command);
        }

        private void ListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var theList = sender as ListBox;

            string theName = theList.Name;

            string command = string.Format("Patch:LFO[" + _LFOID + "]." + theName + " = {0}", ((System.Windows.Controls.ContentControl)(theList.SelectedItem)).Content);

            MainWindow.MainWindowSingleTon.SendSShCommand(command);
        }
    }
}
