#include <bits/stdc++.h>
#include <fstream>

using namespace std;

class PacketInfo;
class FileRead;
class FileWrite;

struct Packet
{
    string packet;
    int packetNo;
    string destinationAddress;
    string sourceAddress;
    string type;
    string CRC;
    map<string, string> data;

};




class FileWrite
{
private:
    Packet packet;
    fstream myFile;
public:
    FileWrite()
    {

    }
    FileWrite(Packet packet)
    {
        this->packet = packet;
        myFile.open("output_packets" , ios::app);
    }

    void outputFile()
    {


        if(myFile.is_open())
        {
            string outputString;
            outputString = "Packet #" + to_string(packet.packetNo) + ": " + packet.packet + "\n" +
                           "CRC: " + packet.CRC + "\n" +
                           "Destination Address :" + packet.destinationAddress + "\n" +
                           "Payload size :" + packet.data["Payload Size : "] + "\n" +
                           "Source Address: " +packet.sourceAddress + "\n" +
                           "Type: " +  packet.type + "\n";
            if(packet.type == "AEFE")
            {
                outputString += "Concatenation Indicator: " + packet.data["Concatenation Indicator: "] + "\n" +
                                "Message Type : " + packet.data["Message Type : "] + "\n" +
                                "Protocol Version : " + packet.data["Protocol Version : "] + "\n" +
                                "RTC ID : " + packet.data["RTC ID : "] + "\n" +
                                "Sequence ID: " + packet.data["Sequence ID: "] + "\n";

            }
            outputString += "\n****************************************************************************************\n";
            myFile << outputString;

        }
    }
    void ClearFile()
    {
        myFile.open("output_packets", std::ofstream::out | std::ofstream::trunc);
        myFile.close();

    }
};


class PacketInfo
{
private:
    string packet;
    static int packetNumber;
    int ParsingCounter = 16;
    int addressBits = 12;
    int typeBits = 4;
    int FCS = 8;
public:

    PacketInfo(string packet)
    {
        this->packet = packet;
    }
    string setPacket(string packet)
    {
        this->packet = packet;
        return packet;
    }

    static int getNumber()
    {
        return packetNumber++;
    }


    void getPacketDetails()
    {
        //Counter for looping over the packet after ignoring the 16 bit Preamble
        Packet PacketDetails;
        PacketDetails.packet = packet;
        PacketDetails.packetNo = getNumber();


        PacketDetails.destinationAddress = packet.substr(ParsingCounter , addressBits);
        ParsingCounter += addressBits;

        PacketDetails.sourceAddress = packet.substr(ParsingCounter , addressBits);
        ParsingCounter += addressBits;

        PacketDetails.type = packet.substr(ParsingCounter , typeBits);
        ParsingCounter += typeBits;

        if(PacketDetails.type == "AEFE")
        {
            PacketDetails.data.insert({"Protocol Version : " , packet.substr(ParsingCounter ,1) });
            ParsingCounter += 2;

            PacketDetails.data.insert({ "Message Type : ", packet.substr(ParsingCounter , 2) });
            ParsingCounter += 2;

            PacketDetails.data.insert( { "Payload Size : ", packet.substr(ParsingCounter , 4) });
            ParsingCounter += 4;

            PacketDetails.data.insert({ "RTC ID : " , packet.substr(ParsingCounter , 4) });
            ParsingCounter += 4;

            PacketDetails.data.insert({ "Sequence ID: " , packet.substr(ParsingCounter , 4) });
            PacketDetails.data.insert( {"Concatenation Indicator: " , "0"});

        }

        PacketDetails.CRC = packet.substr(packet.length()-FCS , packet.length());

        FileWrite fileWrite(PacketDetails);
        fileWrite.outputFile();
    }

};

int PacketInfo::packetNumber = 0;


class FileRead
{
private:
    string filePath;

public:
    FileRead(string filePath)
    {
        this->filePath = filePath;
    }

    void Fileopen()
    {
        fstream new_file;
        new_file.open(this->filePath , ios::in);

        if(new_file.is_open())
        {
            string PacketLine;
            while(getline(new_file , PacketLine ))
            {
                PacketInfo packetInfo(PacketLine);
                packetInfo.getPacketDetails();
            }
            new_file.close();
        }

    }
};


int main() {

    FileWrite fileWrite;
    fileWrite.ClearFile();

    FileRead fileRead("input_packets");
    fileRead.Fileopen();

    return 0;
}

