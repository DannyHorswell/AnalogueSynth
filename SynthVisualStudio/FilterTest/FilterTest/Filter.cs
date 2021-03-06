using System;
using System.Collections.Generic;
using System.Text;

namespace FilterTest
{
    public class Filter
    {
        public double L { get { return 1.0F; } }
        public double C { get; set;}
        public double R { get; set; }

        // Reverse laplace transform coeffiecnts
        public double Alpha { get; set; }
        public double Beta { get; set; }

        // Sin and cosine components
        public double VzeroComponent { get; set; }
        public double IzeroComponent { get; set; }


        public double Vc { get; set; }
        public double I { get; set; }


        public Filter()
        {
        }


        /// <summary>
        /// These only neew to be calculated once for sample perion
        /// </summary>
        /// <param name="t"></param>
        public void CalculateDeltaTComponents(double t)
        {
            VzeroComponent = GetVzeroComponent(t);
            IzeroComponent = GetIzeroComponent(t);
        }

        /// <summary>
        /// These ned recalculating when f and q change
        /// </summary>
        /// <param name="frequency"></param>
        /// <param name="q"></param>
        public void RecalculateValues(float frequency, float q)
        {
            C = 1 / (2 * Math.PI * frequency);
            C = C * C;

            R = Math.Sqrt(1 / C) / q;

            Alpha = R / 2;
            Beta = Math.Sqrt((1 / C) + (R * R) / 4);
        }

        public double GetVzeroComponent(double t)
        {
            var eBit = Math.Exp(-Alpha * t);
            var sinBit = Math.Sin(Beta  * t);

            return (eBit * sinBit) / Beta;
        }

        public double GetIzeroComponent(double t)
        {
            var eBit = Math.Exp(-Alpha * t);
            var cosBit = Math.Cos(Beta * t);

            return eBit * cosBit;
        }


        public double Reclaculate(double detlaT, double inputV)
        {
            var vdiff = inputV - Vc;

            var i = vdiff * VzeroComponent + I * IzeroComponent;

            // Store I for next iteration
            I = i;

            // Calculate diference in Vc
            var dv = (I * detlaT) / C;

            // Store Vc for next iteration
            Vc += dv;

            return Vc;
        }

    }
}
