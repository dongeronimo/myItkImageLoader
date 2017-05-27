Biblioteca de carga de dicom usando a ITK

Encapsula meus mecanismos de carga de dicoms usando a itk, para uso em meus projetos.
A primeira etapa é fornecer o diretório onde estão as imagens. Sempre lembrando que 
um diretório pode ter mais de uma imagem. O resultado dessa etapa é uma lista de lista
de string, onde as fatias que compôem a imagem estão agrupadas por lista. A segunda etapa
é a carga da imagem usando uma das listas geradas na primeira etapa.