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
    /// Interaction logic for Octive.xaml
    /// </summary>
    public partial class Octive : UserControl
    {
        public Octive()
        {
            InitializeComponent();
        }

        public int LastKeyIDPressed { get; set; }

        private void Key_Click(object sender, RoutedEventArgs e)
        {
            LastKeyIDPressed = int.Parse(((Button)sender).Name.Replace("Key_", ""));

            if (NotePressed != null)
            {
                NotePressed(this, new EventArgs());
            }
        }


        public event EventHandler NotePressed;
    }
}
