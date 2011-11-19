#include "Config.h"
#include <iostream>

Config::Config()
{
    //ctor
}

Config::~Config()
{
    //dtor
}

const bool Config::Init(int argc, char** argv)
{
    if(argc != 2)
    {
        std::cout<<"Usage: luabindoc <config file>"<<std::endl;
        return false;
    }
    return LoadFromFile(argv[1]);
}

const bool Config::LoadFromFile(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    if(file.fail())
    {
        std::cout<<"Could not open \""<<filename<<"\"!"<<std::endl;
        return false;
    }
    //read blocks until the file is over
    while(!file.eof())
    {
        if(!ReadBlock(file)) return false;
    }
    return true;
}

/** \brief Whether the current block is over, i.e. either a new block starts or the end of the file has been reached **/
static const bool IsBlockOver(std::ifstream& file)
{
    return file.eof() || file.peek() == '[';
}

/** \brief Skips all successive empty lines in the file. **/
static void SkipEmptyLines(std::ifstream& file)
{
    while(!file.eof() && file.peek() == '\n')
    {
        file.get();
    }
}

static void ReadIntoList(std::ifstream& file, std::list<std::string>& out_list)
{
    while(!IsBlockOver(file))
    {
        std::string filename;
        std::getline(file, filename);
        out_list.push_back(filename);
        SkipEmptyLines(file);
    }
}

const bool Config::ReadBlock(std::ifstream& file)
{
    //skip to next block
    while(!IsBlockOver(file))
    {
        //i.e. ignore all lines in current one
        std::string _;
        std::getline(file, _);
    }
    if(file.eof())
    {
        std::cout<<"Config file contains no block!"<<std::endl;
        return false;
    }
    //read block
    std::string blocktype;
    std::getline(file, blocktype);
    //skip to first valid entry
    SkipEmptyLines(file);
    //handle accordingly
    if(blocktype == "[files]")
    {
        //source file block
        std::list<std::string>::size_type prevSize = mSourceFiles.size();
        ReadIntoList(file, mSourceFiles);
        if(prevSize == mSourceFiles.size())
        {
            std::cout<<"Warning: Empty [files] block!"<<std::endl;
        }
    }
    else if(blocktype == "[includedirs]")
    {
        //include dir block
        std::list<std::string>::size_type prevSize = mIncludeDirs.size();
        ReadIntoList(file, mIncludeDirs);
        if(prevSize == mIncludeDirs.size())
        {
            std::cout<<"Warning: Empty [includedirs] block!"<<std::endl;
        }
    }
    else if(blocktype == "[outputdir]")
    {
        //output block
        if(IsBlockOver(file))
        {
            std::cout<<"Error: Empty [outputdir] block!"<<std::endl;
            return false;
        }
        std::getline(file, mOutputDir);
        SkipEmptyLines(file);
        if(!IsBlockOver(file))
        {
            std::cout<<"Warning: More than one output directory specified, ignoring all but first."<<std::endl;
        }
        while(!IsBlockOver(file))
        {
            std::string _;
            std::getline(file, _);
        }
    }
    else
    {
        //unkown block
        std::cout<<"Warning: unknown block type \"" << blocktype << "\", skipping."<<std::endl;
        while(!IsBlockOver(file))
        {
            std::string _;
            std::getline(file, _);
        }
    }
    return true;
}
