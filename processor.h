#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <string>
#include <cmath>
#include <vector> // Dynamic array of data points
#include <fstream>
#include <bitset>
#include <sstream>

#include "ua_law.h"

#define PI 3.141592653589793238462643383279502884

using namespace std;

// SAMPLING =======================================
float f = 500; // Source frequency
float a = 1; // Source amplitude 
int fs = 16000; // Sampling frequency
int s = 5; // Signal duration (seconds)
// QUANTIZING =====================================
int b = 16; // Bit depth

class SineOscillator 
{
    float frequency, amplitude, angle = 0.0f, offset = 0.0f;
public:
    SineOscillator(float freq, float amp) : frequency(freq), amplitude(amp) {
        offset = 2 * PI * frequency / fs;
    }
    float process() {
        auto sample = amplitude * sin(angle);
        angle += offset;
        return sample;
        // Asin(2pif/sr)
    }
};

class Processor: public UA_CODEC
{
public:  
    vector<float> sampled_data;
    vector<int> processed_data; // For testing
    vector<string> PCMCODE;        // Showing RAW PCM DATA
    vector<int> PCMDECODE;
    vector<int> ACODE;          // Showing A-LAW DATA
    vector<int> ADECODE;         
    vector<int> MUCODE;         // Showing MU-LAW DATA
    vector<int> MUDECODE;

public:
    void writeToFile(ofstream &file, int value, float size) 
    {
        file.write(reinterpret_cast<const char*> (&value), size);
    }

    void process()
    {
        cout << "Signal f: " << f << endl;
        cout << "Sampling duration: " << fs << endl;
        cout << "Signal time: " << s << endl;
        cout << "Bit depth: " << b << endl;
        
        //ofstream test;
        //test.open("C:\\Users\\Vincent Joshua\\Documents\\AAAAA\\University\\Fourth Year\\Group Design\\Sound samples\\pcm16_8.bin",ios::binary);

        SineOscillator SineWave(f,a);
        int max = pow(2,b-1)-1; 
        
        for(int t=0;t<fs*s;t++)
        {
            auto sample = SineWave.process(); // Data points for 1 second
            sampled_data.push_back(sample);
            int quantize = static_cast<int>(sample * max);
            /*
            if(b<10)
            {
                int binsize = 1;
                writeToFile(test,quantize,binsize);
            }
            else
            {
                int binsize = 2;
                writeToFile(test,quantize,binsize);
            }*/
            //quantize = quantize + 126; // Unsigned int 8-bit
            //int compressed = MuLaw_Encode(quantize);
            //int decompressed = MuLaw_Decode(compressed);
            
            processed_data.push_back(quantize); // For PCM
        }
        cout << "Signal processed!" << endl;
        cout << "Binary file created" << endl;
        createWAV();
    }

    void createWAV()
    {
        ofstream test;
        test.open("C:\\Users\\Vincent Joshua\\Documents\\AAAAA\\University\\Fourth Year\\Group Design\\Sound samples\\test.wav",ios::binary);

        //Header chunk
        test << "RIFF"; // RIFF Header
        test << "----"; 
        test << "WAVE"; // WAVE Header

        // Format chunk
        test << "fmt "; // FMT Header
        writeToFile(test, 16, 4); // Size of the fmt chunk
        writeToFile(test, 1, 2); // Compression code 1 = PCM, 2 = Microsoft ADPCM, 6 = ULAW, 7 = ALAW, 
        writeToFile(test, 1, 2); // Number of channels 1 = Mono, 2 = Stereo
        writeToFile(test, fs, 4); // Sample rate (Hz)
        writeToFile(test, fs * b / 8, 4 ); // Byte rate 
        writeToFile(test, b / 8, 2); // Block align 2 = 16-bit mono, 4 = 16-bit stereo
        writeToFile(test, b, 2); // Bit depth (Number of bits per sample)

        //Data chunk
        test << "data"; // DATA string 
        test << "----"; 

        int preAudioPosition = test.tellp();
        for(int i=0;i<processed_data.size();i++)
        {
            if(b<10)
            {
                writeToFile(test,processed_data[i],1);
            }
            else
            {
                writeToFile(test,processed_data[i],2); // Data content 
            }
        }    
        int postAudioPosition = test.tellp();

        test.seekp(preAudioPosition - 4);
        writeToFile(test, postAudioPosition - preAudioPosition, 4);

        test.seekp(4, ios::beg);
        writeToFile(test, postAudioPosition - 8, 4);
        
        test.close();

        cout << "WAV file created!" << endl;
        
    }

    string PCMencoding(int quantized) // 16-bit signed string test
    {
        string binary = std::bitset<16>(quantized).to_string(); //to binary
        return binary;
    }

    string PCM_8bitencoding(int quantized) // 16-bit signed string test
    {
        string binary = std::bitset<8>(quantized).to_string(); //to binary
        return binary;
    }

    // function to convert decimal to binary UNSIGNED
    string decToBinary(int n)
    {
        std::string r;
        while(n!=0) {r=(n%2==0 ?"0":"1")+r; n/=2;}
        //cout << r << endl;
        return r;
    }

    int PCMdecoding(string pcm)
    {
        int decimal = std::bitset<16>(pcm).to_ulong();
        if(decimal>32767)
        {
            decimal = decimal - 65536;
        }

        return decimal;
    }

    void pcmEncodeSignal()
    {
        for(int i=0;i<processed_data.size();i++)
        {
            PCMCODE.push_back(PCMencoding(processed_data[i]));
        }
        ofstream dataWrite;
        dataWrite.open("C://Users//Vincent Joshua//Documents//AAAAA//University//Fourth Year//Group Design//Sound samples//pcmencodedsine.txt");

        for(int i=0;i<PCMCODE.size();i++)
        {
            dataWrite << PCMCODE[i] << " ";
        }
        cout << "PCM encoded file created" << endl;

        dataWrite.close();
    }

    void pcmDecodeSignal()
    {
        for(int i=0;i<PCMCODE.size();i++)
        {
            PCMDECODE.push_back(PCMdecoding(PCMCODE[i]));
        }
        ofstream dataWrite;
        dataWrite.open("C://Users//Vincent Joshua//Documents//AAAAA//University//Fourth Year//Group Design//Sound samples//pcmendecodedsine.txt");

        for(int i=0;i<PCMDECODE.size();i++)
        {
            dataWrite << PCMDECODE[i] << " ";
        }
        cout << "PCM decoded file created" << endl;

        dataWrite.close();
    }

    void a_lawEncodeSignal()
    {
        for(int i=0;i<processed_data.size();i++)
        {
            ACODE.push_back(ALaw_Encode(processed_data[i]));
        }
        ofstream dataWrite;
        dataWrite.open("C://Users//Vincent Joshua//Documents//AAAAA//University//Fourth Year//Group Design//Sound samples//alawencodedsine.txt");

        for(int i=0;i<ACODE.size();i++)
        {
            dataWrite << ACODE[i] << " ";
        }
        cout << "A-Law encoded file created" << endl;

        dataWrite.close();
    }

    void a_lawDecodeSignal()
    {
        for(int i=0;i<ACODE.size();i++)
        {
            ADECODE.push_back(ALaw_Decode(ACODE[i]));
        }

        ofstream dataWrite;
        dataWrite.open("C://Users//Vincent Joshua//Documents//AAAAA//University//Fourth Year//Group Design//Sound samples//alawdecodedsine.txt");

        for(int i=0;i<ADECODE.size();i++)
        {
            dataWrite << ADECODE[i] << " ";
        }
        cout << "A_Law decoded file created" << endl;

        dataWrite.close();
    }

    void mu_lawEncodeSignal()
    {
        for(int i=0;i<processed_data.size();i++)
        {
            MUCODE.push_back(MuLaw_Encode(processed_data[i]));
        }
        ofstream dataWrite;
        dataWrite.open("C://Users//Vincent Joshua//Documents//AAAAA//University//Fourth Year//Group Design//Sound samples//mulawencodedsine.txt");

        for(int i=0;i<MUCODE.size();i++)
        {
            dataWrite << MUCODE[i] << " ";
        }
        cout << "Mu-Law encoded file created" << endl;

        dataWrite.close();
    }

    void mu_lawDecodeSignal()
    {
        for(int i=0;i<MUCODE.size();i++)
        {
            MUDECODE.push_back(MuLaw_Decode(MUCODE[i]));
        }

        ofstream dataWrite;
        dataWrite.open("C://Users//Vincent Joshua//Documents//AAAAA//University//Fourth Year//Group Design//Sound samples//mulawdecodedsine.txt");

        for(int i=0;i<MUDECODE.size();i++)
        {
            dataWrite << MUDECODE[i] << " ";
        }
        cout << "Mu-Law decoded file created" << endl;

        dataWrite.close();
    }

    // Reseting Data ===============================    
    void clearAll()
    {
        processed_data.clear();
        PCMCODE.clear();
        PCMDECODE.clear();
        ACODE.clear();
        ADECODE.clear();
        cout << "All data cleared" << endl;
    }

    // Test Write ==================================
    void testWrite()
    {
        ofstream dataWrite;
        dataWrite.open("C://Users//Vincent Joshua//Documents//AAAAA//University//Fourth Year//Group Design//Sound samples//testsample.txt");

        for(int i=0;i<processed_data.size();i++)
        {
            dataWrite << sampled_data[i] << " ";
        }
        cout << "Test file created" << endl;

        dataWrite.close();
    }

};
#endif