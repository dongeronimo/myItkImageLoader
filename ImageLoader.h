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
#include <vtkCallbackCommand.h>

using namespace std;
typedef vector<string> StringList;
typedef itk::Image<short, 3> ShortImage;

namespace  imageLoader
{
	class MyITKProgressEventSender : public itk::Command
	{
	private:
		HWND targetWindow;
		static HWND staticTargetWindow;
	public:
		static MyITKProgressEventSender *New()
		{
			return new MyITKProgressEventSender();
		}

		void SetHWND(HWND alvo)
		{
			targetWindow = alvo;
			staticTargetWindow = alvo;
		}

		void Execute(itk::Object * caller, const itk::EventObject & event) override
		{
			Execute((const itk::Object *)caller, event);
		}

		void Execute(const itk::Object * caller, const itk::EventObject & event) override
		{
			if (!itk::ProgressEvent().CheckEvent(&event))
			{
				return;
			}
			const itk::ProcessObject * processObject = dynamic_cast< const itk::ProcessObject * >(caller);
			if (!processObject)
			{
				return;
			}
			HWND hwnd = targetWindow;
			unsigned int msg = WM_USER + 1000;
			WPARAM wParam = processObject->GetProgress() * 100;
			LPARAM lParam = 0;
			SendMessage(hwnd, msg, wParam, lParam);
		}

		static void vtkProgressCallback(vtkObject* obj, unsigned long eventId, void* clientData, void* calldata)
		{
			HWND hwnd = staticTargetWindow;
			unsigned int msg = WM_USER + 1000;
			WPARAM wParam = vtkAlgorithm::SafeDownCast(obj)->GetProgress() * 100;
			LPARAM lParam = 0;
			SendMessage(hwnd, msg, wParam, lParam);
		}

		vtkSmartPointer<vtkCallbackCommand> CreateVTKProgressObserver()
		{
			vtkSmartPointer<vtkCallbackCommand> cbk = vtkSmartPointer<vtkCallbackCommand>::New();
			cbk->SetCallback(vtkProgressCallback);
			return cbk;
		}
	};


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
	private:
		MyITKProgressEventSender::Pointer eventSender;
	public:
		ImageLoader(HWND progressEventTarget=nullptr);
		shared_ptr<LoadedImage> Load(StringList fatias, string idExame, string idSerie);
		shared_ptr<LoadedImage> LoadVTI(string vtiPath, string idExame, string idSerie);
	};

	static inline void SaveAsXML(itk::Image<float,3>::Pointer src, std::string file)
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
		finalImage->SetDataScalarTypeToFloat();
		float* srcPtr = src->GetBufferPointer();
		finalImage->SetImportVoidPointer(srcPtr, 1);
		finalImage->Modified();
		finalImage->Update();
		vtkSmartPointer<vtkXMLImageDataWriter> writer = vtkSmartPointer<vtkXMLImageDataWriter>::New();
		writer->SetInputConnection(finalImage->GetOutputPort());
		writer->SetFileName(file.c_str());
		writer->Update();
	}

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

