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
using System.ComponentModel;

namespace SynthControl
{
    /// <summary>
    /// Interaction logic for Keyboard.xaml
    /// </summary>
    public partial class Keyboard : UserControl
    {
        public Keyboard()
        {
            InitializeComponent();
        }

        public void Initalise()
        {
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

                SetEventsAndInitialValues(childVisual);
            }
        }

        void SetSliderUpperAndLowerLevels(Slider TheSlider)
        {
            float topLevel = 12.0F;
            float botLevel = -12.0F;

            switch (TheSlider.Name)
            {
                case "PitchBend":
                    TheSlider.Minimum = botLevel;
                    TheSlider.Maximum = topLevel;
                    TheSlider.Value = 0.0F;
                    break;

                    break;

                default:
                    MessageBox.Show("Bad slider name " + TheSlider.Name);
                    break;
            }
        }

        int LastKeyPressed = -1;

        private void Octive_NotePressed(object sender, EventArgs e)
        {
            var oct = ((Octive)sender);

            int octID = int.Parse(oct.Name.Replace("Oct_", ""));

            int keyNumber = 24 + octID * 12 + oct.LastKeyIDPressed;

            string command = "";

            // Release last key pressed
            if (LastKeyPressed > 0)
            {
                command = string.Format("KeyReleased:" + LastKeyPressed);
                MainWindow.MainWindowSingleTon.SendSShCommand(command);
            }

            // Press new key
            command = string.Format("KeyPressed:" + keyNumber);
            MainWindow.MainWindowSingleTon.SendSShCommand(command);

            LastKeyPressed = keyNumber;
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

            string command = string.Format("Synth:" + theName + " = {0}", value);

            MainWindow.MainWindowSingleTon.SendSShCommand(command);
        }
    }
}
