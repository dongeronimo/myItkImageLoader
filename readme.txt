Biblioteca de carga de dicom usando a ITK

Encapsula meus mecanismos de carga de dicoms usando a itk, para uso em meus projetos.
A primeira etapa � fornecer o diret�rio onde est�o as imagens. Sempre lembrando que 
um diret�rio pode ter mais de uma imagem. O resultado dessa etapa � uma lista de lista
de string, onde as fatias que comp�em a imagem est�o agrupadas por lista. A segunda etapa
� a carga da imagem usando uma das listas geradas na primeira etapa.