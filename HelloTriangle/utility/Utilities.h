#pragma once

#include "../pch.h"

namespace util
{
	//USED
	class DeltaTimer {
	private:
		std::chrono::steady_clock::time_point start; //start of life
		std::chrono::steady_clock::time_point last;	 //last call for delta time
		std::chrono::duration<float> delta;
		float lastdelta;
	public:
		DeltaTimer() : lastdelta(0.0f)
		{
			this->Restart();
		}
		~DeltaTimer()
		{
		}
		const float Delta()
		{
			const std::chrono::steady_clock::time_point now = std::chrono::high_resolution_clock::now();
			delta = now - last;
			last = now;
			lastdelta = delta.count();
			return lastdelta;
		}
		const float LastDelta()const
		{
			return lastdelta;
		}
		void Restart()
		{
			start = std::chrono::high_resolution_clock::now();
			last = start;
		}
		const float GetElapsed()const
		{
			std::chrono::steady_clock::time_point now = std::chrono::high_resolution_clock::now();
			std::chrono::duration<float> elapsed = now - start;
			return elapsed.count();
		}
	};


#if  defined(_WIN32) || defined(_WIN64)

#ifndef LASTERR
#define LASTERR std::string(" Last error: " + std::to_string(GetLastError()))
#endif // !LASTERR

	inline
		void ErrorMessageBox(std::string message, std::string boxname = "error")
	{
		message.append(LASTERR);
		std::wstring msg, name;
		msg.assign(message.begin(), message.end());
		name.assign(boxname.begin(), boxname.end());
		MessageBox(NULL,
			msg.c_str(),
			name.c_str(),
			NULL);
	}
#endif
#if 0
	inline
		const std::string ReadFileAsString(const char* filepath) //puts every line in the strign
	{
		std::ifstream file(filepath);
		if (!file.is_open())
			return { "file" + std::string{filepath} + " could not be opened." };

		std::string return_string = "";
		std::string line = "";
		while (getline(file, line))
		{
			return_string.append(line);
		}

		return return_string;
	}

	inline
		const std::vector<std::string> ReadFileAsStringVector(const char* filepath) //puts every line in a vector.
	{
		std::ifstream file(filepath);
		if (!file.is_open())
			return std::vector<std::string>{ "file" + std::string{ filepath } + " could not be opened." };

		std::vector<std::string> return_vector = {};
		std::string line = "";
		while (getline(file, line))
		{
			return_vector.push_back(line);
		}

		return return_vector;
	}


	template<typename T>
	inline std::string GetAsHex(T input)
	{
		std::stringstream ss;
		ss << std::hex << input;
		return ss.str();
	}

	inline
		std::string GetFileExtension(const std::string _filepath)
	{
		std::string ext{};
		std::size_t pos = _filepath.find_last_of('.');

		if (pos != std::string::npos)
			ext = _filepath.substr(pos, _filepath.length());

		return ext;
	}

	inline
		bool HasFileExtension(const std::string _filepath, const std::string _ext)
	{
		return (GetFileExtension(_filepath) == _ext);
	}

	inline
		void OutputFile(const char* text, const std::string path = "default.txt")
	{
		std::ofstream fstr(path, std::ios::out | std::ios::trunc);
		if (!fstr.is_open())
		{
			std::cerr << "could not open file at path: " << path << std::endl;
		}
		else
		{
			fstr << text;
		}
	}

	inline
		void OutputFile(const std::string text, const std::string path = "default.txt")
	{
		OutputFile(text.c_str(), path);
	}

	inline
		void OutputErrorFile(const std::string errortext, std::string filepath = "error.txt")
	{
		std::string error = "Error: ";
		error.append(errortext + "\n");
		if (filepath.substr(filepath.length() - 4, filepath.length()) != ".txt")
		{
			filepath.append(".txt");
		}
		OutputFile(error, filepath);
	}

	inline
		const std::string getFirstSubstr(const std::string str, const std::string delimiter = " ") //gets the first space-separated token of a given string. 
	{
		if (str.empty()) return "";

		std::string token = str.substr(0, str.find_first_of(" "));

		return token;
	};

	inline
		const std::string cutFirstSubstr(std::string& str, const std::string delimiter = " ") //returns the first delimiter-separated token of a given string and shaves it from the original string,
	{																				//delimiter defaults to space
		if (str.empty()) return "";

		std::string token = str.substr(0, str.find_first_of(delimiter));

		if (token.length() < str.length())
		{
			str = str.substr(token.length() + 1, str.length()); //shave the token from the initial line, remove the delimiter
		}

		return token;
	};

	inline
		void getTail(std::string& main, const std::string token)
	{
		std::string tail;
		//head = main.substr(main.find_first_of(token), token.length());
		tail = main.substr(main.find_first_of(token) + token.length(), main.length());
		while (tail[0] == ' ' || tail[0] == '\t' || tail[0] == '\n' || tail[0] == '\0') //remove spaces, tabs and linebreaks until next real character
		{
			tail = tail.substr(1, tail.length());
		}

		main = tail;

	}

	inline
		std::string getHead(std::string& main, const std::string token)
	{
		std::string head;
		size_t pos = main.find_first_of(token);
		head = main.substr(0, pos);

		if (head.length() > 1)
		{
			while (head[0] == ' ' || head[0] == '\t' || head[0] == '\n' || head[0] == '\0') //remove spaces, tabs and linebreaks until next real character
			{
				head = head.substr(1, head.length());
			}
		}

		main = head;

		return main;
	}


	inline
		const std::vector<std::string> splitString(const std::string str, const std::string token) //splits a given string into a string vector around each occurence of a given token
	{
		std::vector<std::string> strarr{};
		unsigned int n = str.length();
		if (n == 0)
			return strarr;
		if (n == 1)
			return { str };

		size_t offset = 0;
		std::string copy = str;
		std::string temp = "";
		while (copy.find_first_of(token) < n)
		{
			offset = copy.find_first_of(token);
			//skip if token is on first location
			if (offset == 0)
			{
				temp = copy;
			}
			else
			{
				temp = copy.substr(0, offset);
			}
			//remove spaces
			if (temp[0] == ' ')
				temp = temp.substr(1, temp.length());
			if (temp[temp.length()] == ' ')
				temp = temp.substr(0, temp.length() - 1);

			if (temp != "") //don't push empty characters onto return vec
				strarr.push_back(temp);
			//shorten the string
			copy = copy.substr(++offset, n);
		}
		if (copy != "")
			strarr.push_back(copy);

		return strarr;
	}


	inline
		const std::string removeStringToken(std::string& str, const std::string token)//removes each occurence of given token from string
	{
		unsigned int n = str.length();
		if (n <= 1)
			return str;

		size_t start = 0, pos = 0;
		std::string temp = "";
		while (str.find_first_of(token) < n)
		{
			pos = str.find_first_of(token);
			//skip if token is on first location
			if (pos == start)
			{
				temp = str;
			}
			else
			{
				temp = str.substr(start, pos);
			}
			//remove spaces
			if (temp[0] == ' ')
				temp = temp.substr(1, temp.length());
			if (temp[temp.length()] == ' ')
				temp = temp.substr(0, temp.length() - 1);
			//don't push empty characters onto return vec
			if (temp != "")
				str.append(temp);
			//shorten the string
			str = str.substr(++pos, n);
		}
		//if the string is not empty, append the rest of it
		if (str != "")
			str.append(temp);

		return str;
	}

	inline
		const unsigned int ToUInt(const std::string str)
	{
		unsigned int u = 0;
		unsigned int n = str.length();

		if (n > 0)
		{
			//check if number is signed 
			if (str[0] == '-')
				return 0;
			for (unsigned int i = 0; i < n; i++)
			{
				if (str[i] < 48 || str[i] > 57)
					return u;
				u += static_cast<unsigned int>((str[i] - 48) * pow(10, n - (i + 1)));
			}
		}
		return u;
	}
#endif
} // end of namespace