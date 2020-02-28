using System;
using System.IO;

namespace DefinitionSearcher_v1
{
    class Program
    {
        static void Main(string[] args)
        {
            string path = "";                            

            
            do
            {
                Console.Clear();
                Console.WriteLine("Podaj sciezke do folderu: ");
                path = Console.ReadLine();
            }
            while (!isValid(path));




            RecursiveFileProcessor fileProcessor =new RecursiveFileProcessor();
            fileProcessor.ProcessDirectory(path, false);
            fileProcessor.ProcessDirectory(path, true);
            fileProcessor.WriteNames();                                             



        }

        static bool isValid(string filePath)
        {
            if (!Directory.Exists(filePath)) return false;
            return Path.GetExtension(filePath) == "";
        }
    }
}
