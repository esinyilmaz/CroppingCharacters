

#include <iostream>
#include <algorithm>
#include <functional>
#include <boost/filesystem.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/regex.hpp>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <map>
#include <vector>
#include <regex>
#include <cstdlib>

namespace fs = ::boost::filesystem;
namespace io = boost::iostreams;
using namespace std;

//boost::filesystem::path SearchDir("C:\\Users\\e.yilmaz\\Source\\AUSCA0100A");
//boost::filesystem::path SearchDir("C:\\Users\\e.yilmaz\\Source\\CZEID0200A");
//boost::filesystem::path SearchDir("C:\\Users\\e.yilmaz\\Source\\Train Process\\\CZEID0200A\\TrainData");
//boost::filesystem::path SearchDir("C:\\Users\\e.yilmaz\\Source\\Train Process\\\AUSCA0100A\\TrainData");
boost::filesystem::path SearchDir2("C:\\Users\\e.yilmaz\\Source\\Train Process");
boost::filesystem::path SearchDir3("C:\\Users\\e.yilmaz\\Source\\Train Process\\ENGLISH\\TrainedData");

//boost::filesystem::path SearchDir1("C:\\Users\\e.yilmaz\\Source\\Train Document Types");
boost::filesystem::path SearchDir1("C:\\Users\\e.yilmaz\\Source\\training ocr");




// Recursively find the location of a file on a given directory 
void FindFiles(const boost::filesystem::path& directory,
	vector<fs::path>& ret,
	const std::string& filename)
{
	bool found = false;

	const boost::filesystem::path file = filename;
	const boost::filesystem::recursive_directory_iterator end;
	const boost::filesystem::recursive_directory_iterator dir_iter(directory);

	std::find_if(dir_iter, end,
		[&file, &ret](const boost::filesystem::directory_entry& e)
	{
		if (e.path().filename() == file)
			ret.push_back(e.path());
		return false;
	});

}

// Recursively find the location of a file on a given directory 
void FindBoxFiles(const boost::filesystem::path& directory,
	vector<fs::path>& ret)
{
	bool found = false;

	const boost::filesystem::recursive_directory_iterator end;
	const boost::filesystem::recursive_directory_iterator dir_iter(directory);

	std::find_if(dir_iter, end,
		[&ret](const boost::filesystem::directory_entry& e)
	{
		if (e.path().filename().extension() == string(".box"))
			ret.push_back(e.path());
		return false;
	});

}

// Recursively find the location of a file on a given directory 
bool FindFile(const boost::filesystem::path& directory,
	boost::filesystem::path& path,
	const std::string& filename)
{
	bool found = false;

	const boost::filesystem::path file = filename;
	const boost::filesystem::recursive_directory_iterator end;
	const boost::filesystem::recursive_directory_iterator dir_iter(directory);

	const auto it = std::find_if(dir_iter, end,
		[&file](const boost::filesystem::directory_entry& e)
	{
		return e.path().filename() == file;
	});

	if (it != end)
	{
		path = it->path();
		found = true;
	}

	return found;
}

typedef std::vector<cv::Mat> imgListType;

void tokenize(const string& str, vector<string>& tokens, const string& delimiters = ",")
{
	// Skip delimiters at beginning.
	//string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	string::size_type lastPos = 0;

	// Find first non-delimiter.
	//string::size_type pos = str.find_first_of(delimiters, lastPos);
	string::size_type pos = 1;

	while (string::npos != pos || string::npos != lastPos) {
		// Found a token, add it to the vector.

		tokens.push_back(str.substr(lastPos, pos - lastPos));

		// Skip delimiters.
		lastPos = str.find_first_not_of(delimiters, pos);

		// Find next non-delimiter.
		pos = str.find_first_of(delimiters, lastPos);
	}
}
void takeCharListFromTxtFile(std::string path)
{
	static int count = 0;
	//open the file and  read the char positions

	std::cout << "start count" << count << std::endl;
	if (boost::filesystem::file_size(path))
	{
		io::stream<io::mapped_file_source> str(path);

		// you can read from str like from any stream, str >> x >> y >> z

		//read the binarized image
		string pathBinImg = path;
		string pathWImg = path;


		pathBinImg = std::regex_replace(pathBinImg, std::regex("CharPlaces.txt"), "SegBinImg.jpg");
		pathWImg = std::regex_replace(pathWImg, std::regex("CharPlaces.txt"), "White.jpg");



		cv::Mat myBinImg = cv::imread(pathBinImg);
		cv::Mat myWImg = cv::imread(pathWImg);


		vector<string> tokens;
		for (string x; str >> x;)
		{
			//std::cout << "Reading from file: " << x << '\n';
			tokenize(x, tokens);

			string name = "";
			float X = 0;
			float Y = 0;
			float W = 0;
			float H = 0;

			if (tokens.size() == 6)
			{
				// Setup a rectangle to define your region of interest
				tokens[0] = tokens[0] + tokens[1];
				X = std::stof(tokens[2]);
				Y = std::stof(tokens[3]);
				W = std::stof(tokens[4]);
				H = std::stof(tokens[5]);
			}
			else if (tokens.size() == 5)
			{
				// Setup a rectangle to define your region of interest
				//detect illegal path characters
				switch (tokens[0].c_str()[0])
				{
				case '\\': tokens[0] = "slash";
					break;
				case '\*': tokens[0] = "star";
					break;

				case '\?': tokens[0] = "question";
					break;

				case '\:': tokens[0] = "dotdot";
					break;

				case '\"': tokens[0] = "upper";
					break;

				case '\<': tokens[0] = "small";
					break;

				case '\>': tokens[0] = "bigger";
					break;

				case '\|': tokens[0] = "or";
					break;

				case '\/': tokens[0] = "backslash";
					break;
				case '\.': tokens[0] = "dot";
					break;


				default:
					break;
				}
				X = std::stof(tokens[1]);
				Y = std::stof(tokens[2]);
				W = std::stof(tokens[3]);
				H = std::stof(tokens[4]);
			}
			else
			{
				int x = 0;
				x = 4;
			}

			////now crop the image
			//// Setup a rectangle to define your region of interest
			//float X = std::stof(tokens[1]);
			//float Y = std::stof(tokens[2]);
			//float W = std::stof(tokens[3]);
			//float H = std::stof(tokens[4]);


			cv::Rect myROI(X, Y, W, H);

			// Crop the full image to that image contained by the rectangle myROI
			// Note that this doesn't copy the data
			cv::Mat croppedBinImage;
			cv::Mat croppedWImage;
			try
			{
				croppedBinImage = myBinImg(myROI);

			}
			catch (...)
			{
				tokens.clear();
				continue;
			}

			//store it to related folder
			count++;

			string scount = std::to_string(count);

			//store it CHARS_BIN

			/*string cropBinImgPath =
			"C:\\Users\\e.yilmaz\\Source\\For Esin - Copy" +
			path.substr(strlen("C:\\Users\\e.yilmaz\\Source\\For Esin - Copy\\") - 1, 11) + "\\CHARS_BIN\\" ;*/

			string cropBinImgPath = SearchDir1.string() + "\\CHARS_BIN\\";

			boost::filesystem::path dir(cropBinImgPath);

			if (!(boost::filesystem::exists(dir)))
			{
				//std::cout << "Doesn't Exists" << dir << std::endl;

				if (boost::filesystem::create_directory(dir))
					std::cout << "....Successfully Created !" << std::endl;
			}
			cropBinImgPath = cropBinImgPath + tokens[0] + "\\";
			boost::filesystem::path dir2(cropBinImgPath);

			if (!(boost::filesystem::exists(dir2)))
			{
				//std::cout << "Doesn't Exists" << dir2 << std::endl;

				if (boost::filesystem::create_directory(dir2))
					std::cout << "....Successfully Created !" << std::endl;
			}
			cropBinImgPath = cropBinImgPath + scount + ".jpg";
			cv::imwrite(cropBinImgPath, croppedBinImage);

			//store it CHARS_WHITE
			/*string cropWImgPath =
			"C:\\Users\\e.yilmaz\\Source\\For Esin - Copy" +
			path.substr(strlen("C:\\Users\\e.yilmaz\\Source\\For Esin - Copy\\") - 1, 11) + "\\CHARS_WHITE\\";*/
			string cropWImgPath = SearchDir1.string() + "\\CHARS_WHITE\\";

			boost::filesystem::path dir3(cropWImgPath);

			if (!(boost::filesystem::exists(dir3)))
			{
				//std::cout << "Doesn't Exists" << dir3 << std::endl;

				if (boost::filesystem::create_directory(dir3))
					std::cout << "....Successfully Created !" << std::endl;
			}
			cropWImgPath = cropWImgPath + tokens[0] + "\\";
			boost::filesystem::path dir4(cropWImgPath);

			if (!(boost::filesystem::exists(dir4)))
			{
				//std::cout << "Doesn't Exists" << dir4 << std::endl;

				if (boost::filesystem::create_directory(dir4))
					std::cout << "....Successfully Created !" << std::endl;
			}
			cropWImgPath = cropWImgPath + scount + ".jpg";
			cv::imwrite(cropWImgPath, croppedWImage);

			tokens.clear();

		}
	}
	std::cout << "end count" << count << std::endl;

}
void contructBoxFileFromTxtFile(std::string path)
{
	static int count = 0;
	static string oldPath = SearchDir1.string();

	//open the file and  read the char positions

	if (oldPath != SearchDir1.string())
	{
		count = 0;
		oldPath = SearchDir1.string();
	}
	if (boost::filesystem::file_size(path))
	{
		io::stream<io::mapped_file_source> str(path);

		// you can read from str like from any stream, str >> x >> y >> z

		//read the binarized image
		string pathBoxImg = path;
		string pathBinImg = path;
		string pathBinImg2 = path;

		string scount = std::to_string(count);
		count++;

		pathBinImg = std::regex_replace(pathBinImg, std::regex("CharPlaces.txt"), "SegBinImg.jpg");
		/*pathBinImg2 = std::regex_replace(pathBinImg2, std::regex("CharPlaces.txt"), "eng.SegBinImg.exp")
			+ scount + ".jpg";


			pathBoxImg = std::regex_replace(pathBoxImg, std::regex("CharPlaces.txt"), "eng.SegBinImg.exp") + scount + ".box";*/
		string trainPath = SearchDir1.string() + string("\\TrainData");
		pathBinImg2 = trainPath + string("\\eng.SegBinImg.exp") + scount + ".jpg";


		pathBoxImg = trainPath + string("\\eng.SegBinImg.exp") + scount + ".box";

		if (!(boost::filesystem::exists(trainPath)))
		{
			std::cout << "Doesn't Exists" << trainPath << std::endl;

			if (boost::filesystem::create_directory(trainPath))
				std::cout << "....Successfully Created !" << std::endl;
		}

		std::ofstream outfile(pathBoxImg);


		cv::Mat myBinImg = cv::imread(pathBinImg);
		cv::imwrite(pathBinImg2, myBinImg);

		vector<string> tokens;
		for (string x; str >> x;)
		{
			tokenize(x, tokens);


			float X = 0;
			float Y = 0;
			float W = 0;
			float H = 0;
			string name = "";

			if (tokens.size() == 6)
			{
				// Setup a rectangle to define your region of interest
				name = tokens[0] + tokens[1];
				X = std::stof(tokens[2]);
				Y = std::stof(tokens[3]);
				W = std::stof(tokens[4]);
				H = std::stof(tokens[5]);
			}
			else if (tokens.size() == 5)
			{
				// Setup a rectangle to define your region of interest
				name = tokens[0];
				X = std::stof(tokens[1]);
				Y = std::stof(tokens[2]);
				W = std::stof(tokens[3]);
				H = std::stof(tokens[4]);
			}
			else
			{
				int x = 0;
				x = 4;
			}


			float X1 = X;
			float Y1 = myBinImg.size().height - Y - H;
			float X2 = X + W;
			float Y2 = myBinImg.size().height - Y;

			outfile << name.c_str() << " " << X1 << " " << Y1 << " " << X2 << " " << Y2 << "\n";

			tokens.clear();

		}
		outfile.close();

	}

}

void 	takeCharListFromBoxFile(std::string path)
{
	static int count = 0;
	//open the file and  read the char positions

	std::cout << "start count" << count << std::endl;
	if (boost::filesystem::file_size(path))
	{
		io::stream<io::mapped_file_source> str(path);

		// you can read from str like from any stream, str >> x >> y >> z

		//read the binarized image
		string pathBinImg = path;

		pathBinImg = std::regex_replace(pathBinImg, std::regex("box"), "jpg");

		cv::Mat myBinImg = cv::imread(pathBinImg);


		for (string name; str >> name;)
		{
			float X1 = 0;
			float Y1 = 0;
			float X2 = 0;
			float Y2 = 0;
			str >> X1 >> Y1 >> X2 >> Y2;

			//if (tokens.size() == 6)
			//{
			//	// Setup a rectangle to define your region of interest
			//	tokens[0] = tokens[0] + tokens[1];
			//	X1 = std::stof(tokens[2]);
			//	Y1 = std::stof(tokens[3]);
			//	X2 = std::stof(tokens[4]);
			//	Y2 = std::stof(tokens[5]);
			//}
			//else if (tokens.size() == 5)
			//{
			//	// Setup a rectangle to define your region of interest
			//	//detect illegal path characters
			//	switch (tokens[0].c_str()[0])
			//	{
			//	case '\\': tokens[0] = "slash";
			//		break;
			//	case '\*': tokens[0] = "star";
			//		break;

			//	case '\?': tokens[0] = "question";
			//		break;

			//	case '\:': tokens[0] = "dotdot";
			//		break;

			//	case '\"': tokens[0] = "upper";
			//		break;

			//	case '\<': tokens[0] = "small";
			//		break;

			//	case '\>': tokens[0] = "bigger";
			//		break;

			//	case '\|': tokens[0] = "or";
			//		break;

			//	case '\/': tokens[0] = "backslash";
			//		break;


			//	default:
			//		break;
			//	}
			//	X1 = std::stof(tokens[1]);
			//	Y1 = std::stof(tokens[2]);
			//	X2 = std::stof(tokens[3]);
			//	Y2 = std::stof(tokens[4]);
			//}
			//else
			//{
			//	int x = 0;
			//	x = 4;
			//}

			cv::Rect myROI(X1, myBinImg.size().height - Y2, X2 - X1, Y2 - Y1);

			// Crop the full image to that image contained by the rectangle myROI
			// Note that this doesn't copy the data
			cv::Mat croppedBinImage;

			try
			{
				croppedBinImage = myBinImg(myROI);

			}
			catch (...)
			{
				continue;
			}

			//store it to related folder
			count++;

			string scount = std::to_string(count);

			//store it CHARS_BIN

			/*string cropBinImgPath =
			"C:\\Users\\e.yilmaz\\Source\\For Esin - Copy" +
			path.substr(strlen("C:\\Users\\e.yilmaz\\Source\\For Esin - Copy\\") - 1, 11) + "\\CHARS_BIN\\" ;*/

			string cropBinImgPath = SearchDir2.string() + "\\BOX_CHARS_BIN\\";

			boost::filesystem::path dir(cropBinImgPath);

			if (!(boost::filesystem::exists(dir)))
			{
				//std::cout << "Doesn't Exists" << dir << std::endl;
				boost::filesystem::create_directory(dir);
				//if (boost::filesystem::create_directory(dir))
				//std::cout << "....Successfully Created !" << std::endl;
			}


			switch (name.c_str()[0])
			{
			case '\\': name = "slash";
				break;
			case '\*': name = "star";
				break;

			case '\?': name = "question";
				break;

			case '\:': name = "dotdot";
				break;

			case '\"': name = "upper";
				break;

			case '\<': name = "small";
				break;

			case '\>': name = "bigger";
				break;

			case '\|': name = "or";
				break;

			case '\/': name = "backslash";
				break;
			case '\.': name = "dot";
				break;


			default:
				break;
			}
			cropBinImgPath = cropBinImgPath + name + "\\";
			boost::filesystem::path dir2(cropBinImgPath);

			if (!(boost::filesystem::exists(dir2)))
			{
				//std::cout << "Doesn't Exists" << dir2 << std::endl;
				boost::filesystem::create_directory(dir2);
				/*if (boost::filesystem::create_directory(dir2))
					std::cout << "....Successfully Created !" << std::endl;*/
			}
			cropBinImgPath = cropBinImgPath + scount + ".jpg";
			cv::imwrite(cropBinImgPath, croppedBinImage);

		}
	}
	std::cout << "end count" << count << std::endl;
}


void 	takeCharListFromBoxFiletif(std::string path)
{
	static int count = 0;
	//open the file and  read the char positions

	std::cout << "start count" << count << std::endl;
	if (boost::filesystem::file_size(path))
	{
		io::stream<io::mapped_file_source> str(path);

		// you can read from str like from any stream, str >> x >> y >> z

		//read the binarized image
		string pathBinImg = path;

		pathBinImg = std::regex_replace(pathBinImg, std::regex("box"), "tif");

		std::vector< cv::Mat > 	mats;
		cv::imreadmulti(pathBinImg, mats);

		for (string name; str >> name;)
		{
			float X1 = 0;
			float Y1 = 0;
			float X2 = 0;
			float Y2 = 0;
			int   page = 0;
			str >> X1 >> Y1 >> X2 >> Y2 >> page;


			cv::Rect myROI(X1, mats[page].size().height - Y2, X2 - X1, Y2 - Y1);

			// Crop the full image to that image contained by the rectangle myROI
			// Note that this doesn't copy the data
			cv::Mat croppedBinImage;

			try
			{
				croppedBinImage = mats[page](myROI);

			}
			catch (...)
			{
				continue;
			}

			//store it to related folder
			count++;

			string scount = std::to_string(count);


			string cropBinImgPath = SearchDir3.string() + "\\BOX_CHARS_BIN\\";

			boost::filesystem::path dir(cropBinImgPath);

			if (!(boost::filesystem::exists(dir)))
			{
				//std::cout << "Doesn't Exists" << dir << std::endl;
				boost::filesystem::create_directory(dir);
				//if (boost::filesystem::create_directory(dir))
				//std::cout << "....Successfully Created !" << std::endl;
			}


			switch (name.c_str()[0])
			{
			case '\\': name = "slash";
				break;
			case '\*': name = "star";
				break;

			case '\?': name = "question";
				break;

			case '\:': name = "dotdot";
				break;

			case '\"': name = "upper";
				break;

			case '\<': name = "small";
				break;

			case '\>': name = "bigger";
				break;

			case '\|': name = "or";
				break;

			case '\/': name = "backslash";
				break;
			case '\.': name = "dot";
				break;


			default:
				break;
			}
			cropBinImgPath = cropBinImgPath + name + "\\";
			boost::filesystem::path dir2(cropBinImgPath);

			if (!(boost::filesystem::exists(dir2)))
			{
				//std::cout << "Doesn't Exists" << dir2 << std::endl;
				boost::filesystem::create_directory(dir2);
				/*if (boost::filesystem::create_directory(dir2))
				std::cout << "....Successfully Created !" << std::endl;*/
			}
			cropBinImgPath = cropBinImgPath + scount + ".jpg";
			cv::imwrite(cropBinImgPath, croppedBinImage);

		}
	}
	std::cout << "end count" << count << std::endl;
}

void GetCharfromBoxFile(std::string path, std::string ch)
{
	std::string pathnew = SearchDir3.string() + "\\" + "missingChars.box";
	//ofstream ofs(pathnew, ios::app);
	FILE* file = fopen(pathnew.c_str(), "ab");
	
	if (boost::filesystem::file_size(path))
	{
		io::stream<io::mapped_file_source> str(path);

		for (string name; str >> name;)
		{
			int X1 = 0;
			int Y1 = 0;
			int X2 = 0;
			int Y2 = 0;
			int   page = 0;
			str >> X1 >> Y1 >> X2 >> Y2 >> page;

			if (name == ch)
			{
				// put the result file  

				/*ofs << name << " " << X1 << " " << Y1 << " " << X2 << " " << Y2 << " " << page;
				ofs.put('\x0A');*/
				fprintf(file, "%s %d %d %d %d %d\n", name.c_str(), X1, X2, Y1, Y2, page);
			}
		}
	}
	fclose(file);
}

void GetCharfromTRFile(std::string path, std::string fn, std::string ch)
{
	std::string pathnew = SearchDir3.string() + "\\" + "missingChars.tr";
	//ofstream ofs(pathnew, ios::app);
	FILE* file = fopen(pathnew.c_str(), "ab");

	path = std::regex_replace(path, std::regex("box"), "tr");
	//open the file and  read the char positions

	if (boost::filesystem::file_size(path))
	{
		io::stream<io::mapped_file_source> str(path);
		string name; str >> name;

		//if the name is fontname, compare for character match
		do
		{
			again:
			if (name == fn)
			{
				string rch;
				str >> rch;
				if (rch == ch)
				{
					// match also characters read and copy until next font name
					//ofs << name << " " << ch << "  
					fprintf(file, "%s %s ", "SegBinImg", ch.c_str());
					for (std::string x; std::getline(str, x, '\n');)
					{
						//ofs << " "<< x <<"\x0A";
						if (x != "")
							fprintf(file, " %s\n", x.c_str());
						else
							fprintf(file, "\n", x.c_str());
						// read firt word of the line
						str >> name;
						// if it is font name exit from loop
						if (name == fn)
							goto again;
						else
						{
							char* pEnd;
							float f1, f2;
							bool negate = (name[0] == '-');
							
							int isNumber = negate || std::isdigit(name[0]);
							if (isNumber)
								//ofs << " " <<name;
								fprintf(file, " %s", name.c_str());
							else
								//ofs << name;
							    fprintf(file, "%s", name.c_str());
						}

					}
					break;
				}
				else
				{
					// skip to until next font name
					for (name; str >> name;)
					{
						if (name == fn)
						{
							goto again;
						}
							
					}
					break;
				}

			}
		} 
		while (true);

	}
}

int main(int argc, char** argv)
{
	std::vector<boost::filesystem::path> ret;
	int taskId = 1;
	cout << "Enter the task to do" << endl;
	cout << "1-generate box file from char places txt" << endl;
	cout << "2-generate character folders from box file" << endl;
	cout << "3-generate character folders from tif box file" << endl;
	cout << "4-get characters from box and tr files" << endl;

	cin >> taskId;

	if (taskId == 1)
	{
		FindFiles(SearchDir1, ret, "CharPlaces.txt");
		for (auto i = ret.begin(); i != ret.end(); i++)
		{
			std::cout << i->string() << std::endl;
			// start to take character places
			SearchDir1 = i->parent_path().parent_path();
			takeCharListFromTxtFile(i->string());
			contructBoxFileFromTxtFile(i->string());
		}
	}

	else if (taskId == 2)
	{
		//Takes all characters after box update
		FindBoxFiles(SearchDir2, ret);
		for (auto i = ret.begin(); i != ret.end(); i++)
		{

			std::cout << i->filename() << std::endl;

			SearchDir2 = i->parent_path();
			// start to take character places
			takeCharListFromBoxFile(i->string());

		}

	}
	else if (taskId == 3)
	{
		//Takes all characters after box update
		FindBoxFiles(SearchDir3, ret);
		for (auto i = ret.begin(); i != ret.end(); i++)
		{

			std::cout << i->filename() << std::endl;

			SearchDir3 = i->parent_path();

			takeCharListFromBoxFiletif(i->string());
		}

	}
	else
	{
		// get characters from the box and tr  files
		//Takes all characters after box update
		FindBoxFiles(SearchDir3, ret);
		for (auto i = ret.begin(); i != ret.end(); i++)
		{

			if ((i->filename() != "missingChars.box") && (i->filename() != "missingChars.tr"))
			{
				std::cout << i->filename() << std::endl;

				SearchDir3 = i->parent_path();

				if (argc == 3)
				{
					GetCharfromBoxFile(i->string(), std::string(argv[2]));
					GetCharfromTRFile(i->string(), std::string(argv[1]), std::string(argv[2]));
				}

			}

		}
	}

	return 0;
}