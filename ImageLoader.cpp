#include "ImageLoader.h"
#include <itkImage.h>
#include <itkImageSeriesReader.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkOrientImageFilter.h>
using namespace imageLoader;

template<typename TImage> void ItkImageDeepCopy(typename TImage::Pointer input, typename TImage::Pointer output)
{
	int inSzX = input->GetLargestPossibleRegion().GetSize()[0];
	int inSzY = input->GetLargestPossibleRegion().GetSize()[1];
	int inSzZ = input->GetLargestPossibleRegion().GetSize()[2];
	double inSpacingX = input->GetSpacing()[0];
	double inSpacingY = input->GetSpacing()[1];
	double inSpacingZ = input->GetSpacing()[2];

	output->SetRegions(input->GetLargestPossibleRegion());
	output->Allocate();

	itk::ImageRegionConstIterator<TImage> inputIterator(input, input->GetLargestPossibleRegion());
	itk::ImageRegionIterator<TImage> outputIterator(output, output->GetLargestPossibleRegion());
	const double inSpacing[] = { inSpacingX, inSpacingY, inSpacingZ };
	output->SetSpacing(inSpacing);
	while (!inputIterator.IsAtEnd())
	{
		outputIterator.Set(inputIterator.Get());
		++inputIterator;
		++outputIterator;
	}
}

vector<StringList> NameListGenerator::CreateList(string dirPath, StringList& outSeriesIdentifiers)
{
	typedef itk::Image< short, 3 >         ImageType;
	typedef itk::ImageSeriesReader< ImageType >        ReaderType;
	ReaderType::Pointer reader = ReaderType::New();
	typedef itk::GDCMImageIO       ImageIOType;
	ImageIOType::Pointer dicomIO = ImageIOType::New();
	typedef itk::GDCMSeriesFileNames NamesGeneratorType;
	typedef std::vector< std::string >    SeriesIdContainer;
	typedef std::vector< std::string >   FileNamesContainer;
	const char* source_dir = dirPath.c_str();
	vector<StringList> result;
	reader->SetImageIO(dicomIO);
	NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
	nameGenerator->SetUseSeriesDetails(true);
	nameGenerator->SetDirectory(source_dir);
	const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
	int i = 0;
	SeriesIdContainer::const_iterator seriesListIt = seriesUID.begin();
	SeriesIdContainer::const_iterator seriesListEndId = seriesUID.end();
	while (seriesListIt != seriesListEndId)
	{
		StringList currentList;
		std::string seriesIdentifier = seriesListIt->c_str();
		outSeriesIdentifiers.push_back(seriesIdentifier);
		FileNamesContainer fileNames = nameGenerator->GetFileNames(seriesIdentifier);
		reader->SetFileNames(fileNames);
		for (int i = 0; i < fileNames.size(); i++)
		{
			std::string _s = fileNames[i];
			currentList.push_back(_s);
		}
		++seriesListIt;
		result.push_back(currentList);
		i++;
	}
	return result;
}

bool LoadedImage::IsThisExam(string idExame, string idSerie)
{
	if (this->idExame == idExame && this->idSerie == idSerie)
		return true;
	else
		return false;
}

string LoadedImage::GetIdExame()
{
	return idExame;
}

string LoadedImage::GetIdSerie()
{
	return idSerie;
}

LoadedImage::LoadedImage(ShortImage::Pointer img, map<string, string> metadata, string idExame, string idSerie)
{
	this->image = img;
	this->metadata = metadata;
	this->idExame = idExame;
	this->idSerie = idSerie;
}

ShortImage::Pointer LoadedImage::GetImage()
{
	return this->image;
}

map<string, string> LoadedImage::GetMetadata()
{
	return this->metadata;
}

shared_ptr<LoadedImage> ImageLoader::Load(StringList fatias, string idExame, string idSerie)
{
	assert("TEM FATIAS???"&&fatias.size() > 0);
	map<string, string> metadata;
	typedef itk::MetaDataDictionary DictionaryType;
	typedef itk::MetaDataObject< std::string > MetaDataStringType;
	typedef short    PixelType;
	const unsigned int      Dimension = 3;
	//typedef itk::Image< PixelType, Dimension >         ImageType;
	typedef itk::ImageSeriesReader< ShortImage >        ReaderType;
	ReaderType::Pointer reader = ReaderType::New();
	typedef itk::GDCMImageIO       ImageIOType;
	ImageIOType::Pointer dicomIO = ImageIOType::New();
	reader->SetImageIO(dicomIO);
	typedef itk::GDCMSeriesFileNames NamesGeneratorType;
	NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
	//antes de sair abrindo tudo e tomar exce��o se algo n�o for encontrado testar para cada arquivo fornecido
	////se ele existe. Se n�o existe, tira da lista
	vector<int> indices_zuados;
	for (unsigned int i = 0; i < fatias.size(); i++)
	{
		string path = fatias[i];
		WIN32_FIND_DATA FindFileData;
		HANDLE handle = FindFirstFileA(path.c_str(), &FindFileData);
		int found = handle != INVALID_HANDLE_VALUE;
		if (found)
		{
			FindClose(handle);
		}
		else
		{
			indices_zuados.push_back(i);
		}
	}
	for (unsigned int i = 0; i < indices_zuados.size(); i++)
	{
		fatias.erase(fatias.begin() + indices_zuados[i]);
	}
	//Pega o tamanho em bytes da s�rie a partir das propriedades da 1a fatia. Esse tamanho ser� usado para ver
	//se a s�rie caber� na mem�ria. Como o sistema est� limitado a 4gb devido a ser 32 bits e uma s�rie sendo 
	//renderizada na tela ocupa 2.7 seu tamanho original o sistema se recusar� a cerregar algo maior que 900mb
	ReaderType::Pointer singleFileReader = ReaderType::New();
	singleFileReader->SetFileName(fatias[0]);
	singleFileReader->Update();

	//L� a s�rie
	reader->SetFileNames(fatias);
	try
	{
		reader->Update();
	}
	catch (itk::ExceptionObject& ex)
	{
		throw ex;
	}
	//ptrImg � o que ser� retornado.
	ShortImage::Pointer ptrImg = reader->GetOutput();
	const DictionaryType& dictionary = dicomIO->GetMetaDataDictionary();
	DictionaryType::ConstIterator metadataDictionaryIterator = dictionary.Begin();
	DictionaryType::ConstIterator metadataDictionaryEnd = dictionary.End();
	std::cout << "tags dicom" << std::endl;
	typedef itk::MetaDataObject< std::string > MetaDataStringType;

	while (metadataDictionaryIterator != metadataDictionaryEnd)
	{
		itk::MetaDataObjectBase::Pointer entry = metadataDictionaryIterator->second;
		MetaDataStringType::Pointer entryValue = dynamic_cast<MetaDataStringType*>(entry.GetPointer());
		if (entryValue)
		{
			string tagkey = metadataDictionaryIterator->first;
			string labelId;
			string tagvalue = entryValue->GetMetaDataObjectValue();
			std::cout << tagkey << " = " << tagvalue << endl;
			metadata.insert(make_pair(tagkey, tagvalue));
		}
		++metadataDictionaryIterator;
	}

	typedef itk::OrientImageFilter<ShortImage, ShortImage> TOrientImageFilter;
	TOrientImageFilter::Pointer reorientador = nullptr;
	reorientador = TOrientImageFilter::New();
	reorientador->SetInput(ptrImg);
	reorientador->UseImageDirectionOn();
	reorientador->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
	reorientador->Update();

	ItkImageDeepCopy<ShortImage>(reorientador->GetOutput(), ptrImg);
	
	return make_shared<LoadedImage>(ptrImg, metadata,  idExame, idSerie);
}
