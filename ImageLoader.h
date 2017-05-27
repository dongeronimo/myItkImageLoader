#pragma once
#include <string>
#include <memory>
#include <vector>
#include "ILoaded.h"
#include <itkImage.h>
#include <map>
using namespace std;
typedef vector<string> StringList;
typedef itk::Image<short, 3> ShortImage;
namespace  imageLoader
{
	/*
	 * O uso �:
	 * 1)pega as listas de listas de fatias com NameListGenerator
	 * 2)passa uma das listas de fatias para o ImageLoader.
	 * 3)pega a imagem gerada no imageloader
	 */

	class NameListGenerator
	{
	public:
		vector<StringList> CreateList(string dirPath, StringList& outSeriesIdentifiers);
	};

	class LoadedImage : public ILoaded
	{
	private:
		string idExame, idSerie;
		ShortImage::Pointer image;
		map<string, string> metadata;
	public:
		bool IsThisExam(string idExame, string idSerie) override;
		string GetIdExame() override;
		string GetIdSerie() override;
		LoadedImage(ShortImage::Pointer img, map<string, string> metadata, string idExame, string idSerie);
		ShortImage::Pointer GetImage();
		map<string, string> GetMetadata();
		virtual ~LoadedImage(){}
	};

	class ImageLoader
	{
	public:
		shared_ptr<LoadedImage> Load(StringList fatias, string idExame, string idSerie);
	};
}

