using System;
using System.IO;

namespace DefinitionSearcher_v1
{
    class Program
    {
        static void Main(string[] args)
        {
            string path = "";                            

            
           



            RecursiveFileProcessor fileProcessor =new RecursiveFileProcessor();

            fileProcessor.ProcessDirectory(Directory.GetCurrentDirectory());
            Console.ReadKey();


        }

        static bool isValid(string filePath)
        {
            if (!Directory.Exists(filePath)) return false;
            return Path.GetExtension(filePath) == "";
        }
    }
}
