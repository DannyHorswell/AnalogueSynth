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

namespace SynthControl
{
    /// <summary>
    /// Interaction logic for TDL.xaml
    /// </summary>
    public partial class TDL : UserControl
    {

        public string Type {get; protected set;}

        public TDL()
        {
            InitializeComponent();
        }

        public void Initalise(string type)
        {
            Type = type;

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
                    SetSliderUpperAndLowerLevels((Slider)childVisual);
                }

                SetEventsAndInitialValues(childVisual);
            }
        }

        void SetSliderUpperAndLowerLevels(Slider TheSlider)
        {
            float topLevel = 1.0F;
            float botLevel = 0.0F;

            switch (Type)
            {
                case "TDP":
                    topLevel = 12.0F;
                    botLevel = -12.0F;
                    break;

                case "TDA":
                    topLevel = 1.0F;
                    botLevel = 0.0F;
                    break;

                case "TDF":
                    topLevel = 12.0F;
                    botLevel = -12.0F;
                    break;
            }

            switch (TheSlider.Name[0])
            {
                // Gain levels
                case 'L':
                case 'S':   // Sustain
                    TheSlider.Minimum = botLevel;
                    TheSlider.Maximum = topLevel;
                    break;

                // Time levels
                case 'T':
                    TheSlider.Minimum = 0.0;
                    TheSlider.Maximum = 30.0;
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

            string command = string.Format("Patch:WG[" + WGID + "]." + this.Name + "." + theName + " = {0}", value);

            MainWindow.MainWindowSingleTon.SendSShCommand(command);
        }

    }
}
