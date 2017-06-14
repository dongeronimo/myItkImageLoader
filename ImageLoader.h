#pragma once
#include <string>
#include <memory>
#include <vector>
#include "ILoaded.h"
#include <itkImage.h>
#include <itkCommand.h>
#include <map>
#include <vtkImageImport.h>
#include <vtkSmartPointer.h>
#include <vtkXMLImageDataWriter.h>

using namespace std;
typedef vector<string> StringList;
typedef itk::Image<short, 3> ShortImage;

namespace  imageLoader
{
	/*
	 * O uso é:
	 * 1)pega as listas de listas de fatias com NameListGenerator
	 * 2)passa uma das listas de fatias para o ImageLoader.
	 * 3)pega a imagem gerada no imageloader
	 */
	////Aquele que gera a lista de nomes necessária para o loader funcionar.
	class NameListGenerator
	{
	public:
		vector<StringList> CreateList(string dirPath, StringList& outSeriesIdentifiers);
	};
	////A imagem com seu nome, seus voxels e sua metadata dicom.
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
	////Responsável por carregar a imagem, recebe uma lista de fatias e retorna uma imagem.
	class ImageLoader
	{
	public:
		shared_ptr<LoadedImage> Load(StringList fatias, string idExame, string idSerie);
		shared_ptr<LoadedImage> LoadVTI(string vtiPath, string idExame, string idSerie);
	};

	static inline void SaveAsXML(ShortImage::Pointer src, std::string file)
	{
		vtkSmartPointer<vtkImageImport> finalImage = vtkSmartPointer<vtkImageImport>::New();

		int szX = src->GetLargestPossibleRegion().GetSize()[0];
		int szY = src->GetLargestPossibleRegion().GetSize()[1];
		int szZ = src->GetLargestPossibleRegion().GetSize()[2];
		double sX = src->GetSpacing()[0];
		double sY = src->GetSpacing()[1];
		double sZ = src->GetSpacing()[2];
		double oX = src->GetOrigin()[0];
		double oY = src->GetOrigin()[1];
		double oZ = src->GetOrigin()[2];
		finalImage->SetDataSpacing(sX, sY, sZ);
		finalImage->SetDataOrigin(oX, oY, oZ);
		finalImage->SetWholeExtent(0, szX - 1, 0, szY - 1, 0, szZ - 1);
		finalImage->SetDataExtentToWholeExtent();
		finalImage->SetDataScalarTypeToShort();
		void* srcPtr = src->GetBufferPointer();
		finalImage->SetImportVoidPointer(srcPtr, 1);
		finalImage->Modified();
		finalImage->Update();
		vtkSmartPointer<vtkXMLImageDataWriter> writer = vtkSmartPointer<vtkXMLImageDataWriter>::New();
		writer->SetInputConnection(finalImage->GetOutputPort());
		writer->SetFileName(file.c_str());
		writer->Update();
	}
}

