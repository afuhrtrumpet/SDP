#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>

//prototype for a frequency matching function
int determineFrequency(int);

int main(void)
{
    //define constants for number of periods to measure and milliseconds before timeout is generated
    int PERIODS = 5;
    int TIMEOUT = 1000;

    //define variable for frequency and initialize it as 0
    int frequency = 0;

    //configure button board and IR receiver
    ButtonBoard buttons( FEHIO::Bank3 );
    DigitalInputPin ir(FEHIO::P0_0);

    //clear LCD, set its font color and display welcome message
    LCD.Clear( FEHLCD::Red );
    LCD.SetFontColor( FEHLCD::Gray);
    LCD.WriteLine("Welcome! Press the middle button to begin.");

    //wait for the middle button to be pressed
    while(!buttons.MiddlePressed());

    //main loop
    do
    {
        //define variables for 5-period time, timeout counter
        int timeoutInterval = 0;
        int time = 0;
        //determines if timeout has been reached in one of the loops
        bool timeout = false;
        //write the frequency to the LCD display
        LCD.Write(frequency);
        LCD.WriteLine(" Hz");
        //wait for an "on" signal from the IR receiver
        while(ir.Value()) {
            //sleep for one millisecond and increment the timeout counter
            Sleep(1);
            timeoutInterval++;
            //if timeout counter has reached the number of milliseconds given by TIMEOUT
            if (timeoutInterval > TIMEOUT) {
                //set frequency to 0 (for no detection), set timeout to true, and exit loop
                frequency = 0;
                timeout = true;
                break;
            }
        }

        //if timeout has occurred, jump to the top of the main loop
        if (timeout) continue;

        //on signal has now occurred, so determine time taken for the number of periods specified by PERIODS
        for (int i = 0; i < PERIODS; i++)
        {
            //wait for an "off" signal from the IR receiver
            while(!ir.Value()) {
                //sleep for one millisecond, increment time by one
                Sleep(1);
                time++;
                //timeout is not applicable to this loop because an on IR signal means that there is a signal detected
            }
            //reset timeout interval
            timeoutInterval = 0;
            //wait for an "on" signal from the IR receiver
            while(ir.Value()) {
                //sleep for one millisecond, increment time by one
                Sleep(1);
                time++;
                //increment timeout counter
                timeoutInterval++;
                //if timeout has been reached
                if (timeoutInterval > TIMEOUT) {
                    //set frequency to 0 (for no detection), set timeout to true, and exit loop
                    frequency = 0;
                    timeout = true;
                    break;
                }
            }
            //if timeout has occurred, exit this loop
            if (timeout) break;
        }
        //if timeout has occurred, jump to the top of the main loop
        if (timeout) continue;
        //calculate frequency measured based on time and number of periods
        int measuredFrequency = 1000 * PERIODS / time;
        //try to match frequency to one of the possible frequencies
        int setFrequency = determineFrequency(measuredFrequency);
        //-1 is returned for no matched frequency, so set frequency to the matched frequency if not -1
        if (setFrequency != -1)
            frequency = setFrequency;
    } while (!buttons.MiddlePressed()); //exit main loop when middle button is pressed again
    //display closing message
    LCD.WriteLine("Stop pressing my buttons!");
    //loop until the Proteus is turned off
    while (true);
    return 0;
}

//This function attempts to match the measured frequency to one of the possible frequencies. If no match is found, it returns -1.
int determineFrequency(int measuredFrequency) {
    //define array of possible frequencies
    int possibleFrequencies[5] = {25, 40, 65, 80, 110};
    //loop once for each frequency
    for (int i = 0; i < 5; i++) {
        //if the measured frequency is 3 Hz or less off from the possible frequency
        if (measuredFrequency > possibleFrequencies[i] - 3 && measuredFrequency < possibleFrequencies[i] + 3)
            //return the matched frequency
            return possibleFrequencies[i];
    }
    //no frequencies were matched, so return -1
    return -1;
}

