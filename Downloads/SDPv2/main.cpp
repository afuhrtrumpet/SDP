#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>


int determineFrequency(int);


int main(void)
{
    int PERIODS = 5;
    int frequency = 0;
    ButtonBoard buttons( FEHIO::Bank3 );
    DigitalInputPin ir(FEHIO::P0_0);

    LCD.Clear( FEHLCD::Black );
    LCD.SetFontColor( FEHLCD::White );
    LCD.WriteLine("Welcome! Press the middle button to begin.");

    while(!buttons.MiddlePressed());

    bool done = false;
    while(!done)
    {
        int timeoutInterval = 0;
        int time = 0;
        bool timeout = false;
        LCD.Write(frequency);
        LCD.WriteLine(" Hz");
        while(ir.Value()) {
            Sleep(1);
            timeoutInterval++;
            if (timeoutInterval > 1000) {
                frequency = 0;
                timeout = true;
                break;
            }
        }
        if (timeout)
            continue;

        for (int i = 0; i < PERIODS; i++)
        {
            while(!ir.Value()) {
                Sleep(1);
                time++;
            }
            while(ir.Value()) {
                Sleep(1);
                time++;
            }
        }
        int measuredFrequency = 1000 * PERIODS / time;
        int setFrequency = determineFrequency(measuredFrequency);
        if (setFrequency != -1)
            frequency = setFrequency;
    }
    return 0;
}

int determineFrequency(int measuredFrequency) {
    int possibleFrequencies[5] = {25, 40, 55, 80, 110};
    for (int i = 0; i < 5; i++) {
        if (measuredFrequency > possibleFrequencies[i] - 3 && measuredFrequency < possibleFrequencies[i] + 3)
            return possibleFrequencies[i];
    }
    return -1;
}

