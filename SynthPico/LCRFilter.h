#pragma once


class LCRFilter
{
private:

	int _wgID;

    signal _deltaT;


    // Inductor, capacitor and resistor values
    signal L = 1.0F;
    signal C;
    signal R;

    // Reverse laplace transform coeffiecnts
    signal Alpha;
    signal Beta;

    // Sin and cosine components
    signal VzeroComponent;
    signal IzeroComponent;

    signal maxFreq = SAMPLE_FREQUENCY / 4;

    // Initial conditions
    stereo Vc;
    stereo I;

    void RecalculateValues(signal frequency, signal q);
public:
    

    void init(signal deltaT);

	void recalculateCoefficients();

    /// <summary>
    /// Get the next sample
    /// </summary>
    /// <param name="x">input</param>
    /// <returns>output</returns>
    inline stereo getNext(stereo x)
    {
        stereo vdiff;
        stereo i;
        stereo dv;
        stereo ret;

        vdiff.left = x.left - Vc.left;
        //vdiff.right = x.right - Vc.right;

        i.left = vdiff.left * VzeroComponent + I.left * IzeroComponent;
        //i.right = vdiff.right * VzeroComponent + I.right * IzeroComponent;


        // Store I for next iteration
        I = i;

        // Calculate diference in Vc
        dv.left = (I.left * _deltaT) / C;
        //dv.right = (I.right * _deltaT) / C;

        // Store Vc for next iteration
        Vc.left += dv.left;
        //Vc.right += dv.right;

        

        switch (GPatch.Filter.FilterType)
        {
            case filtertype::HPF:
            case filtertype::LOWSHELF:
                ret.left = x.left - Vc.left - (i.left * R);
                //ret.right = x.right - Vc.right - (i.right * R);
                break;

            case filtertype::BPF:
            case filtertype::BPF2:
            case filtertype::PEAKINGEQ:
                ret.left =  i.left * R;
                //ret.right =  i.right * R;
                break;

            case filtertype::NOTCH: // Band stop filter
                ret.left = x.left - (i.left * R);
                //ret.right = x.right - (i.right * R);
                break;

                            case filtertype::OFF:
            case filtertype::APF:
            case filtertype::LPF:
            case filtertype::HIGHSHELF:
            default:
                ret = x;
                break;

        }

        ret.right = ret.left;

        return ret;
    }
};

