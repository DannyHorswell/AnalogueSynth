using System;
using System.IO;
using System.Text;

namespace FilterTest
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Hello World!");

            var filter = new Filter();

            var deltaT = 0.00001F;

            filter.RecalculateValues(5000.0F, 50.0F);
            filter.CalculateDeltaTComponents(deltaT);

            var t = 0.0F;

            var sb = new StringBuilder();

            for (int step=0; step<1000; step++)
            {
                var valv = filter.GetVzeroComponent(t);
                var vali = filter.GetIzeroComponent(t);

                sb.AppendLine($"{t},{valv}, {vali}");

                t += deltaT;
            }

            File.WriteAllText(@"C:\Temp\Wav.csv", sb.ToString());

            sb.Clear();
            t = 0.0F;

            double vc;
            double input;

            for (int step = 0; step < 1000; step++)
            {
                input = step % 250 < 125 ? 1.0 : -1.0;

                vc = filter.Reclaculate(deltaT, input);

                sb.AppendLine($"{t},{vc}");

                t += deltaT;
            }

            File.WriteAllText(@"C:\Temp\Wav2.csv", sb.ToString());

        }
    }
}
