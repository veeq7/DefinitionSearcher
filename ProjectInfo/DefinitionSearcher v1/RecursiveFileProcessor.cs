using System;
using System.IO;
using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace DefinitionSearcher_v1
{
    public class RecursiveFileProcessor
    {
        Dictionary<string, Marker> lights = new Dictionary<string, Marker>();

        public RecursiveFileProcessor()
        {

        }

        public void ProcessDirectory(string targetDirectory, bool findReps)
        {
            string[] fileEntries = Directory.GetFiles(targetDirectory);
            foreach (string fileName in fileEntries)
                ProcessFile(fileName, findReps);

            string[] subdirectoryEntries = Directory.GetDirectories(targetDirectory);
            foreach (string subdirectory in subdirectoryEntries)
                ProcessDirectory(subdirectory, findReps);
        }

        public void ProcessFile(string path, bool findReps)
        {

            string[] lines = File.ReadAllLines(path);

            foreach (string line in lines)
            {
                string[] words = Regex.Split(line, @"\s+");
                if (findReps)
                {
                    foreach (var word in words)
                    {
                        var args = word.Split(',');
                        foreach (var arg in args)
                        {
                            if (arg.StartsWith("IDC_") || arg.StartsWith("IDD_"))
                            {
                                if (lights.ContainsKey(arg))
                                {
                                    lights[arg].Reps++;
                                } else
                                {
                                    Console.ForegroundColor = ConsoleColor.Red;
                                    Console.WriteLine("not found key " + arg);
                                    Console.ResetColor();
                                }
                            }
                        }
                    }
                } else
                {
                    if (line.StartsWith("#define"))
                    {
                        if (words.Length >= 3)
                        {
                            lights.Add(words[1], new Marker(words[1], words[2]));

                        }
                    }
                }
            }
        }
        public void WriteNames()
        {
            string MarkerList ="";

            MarkerList += "Powtorzenia: \n";

            int i=1;       
            foreach (var kvLight in lights)
            {  
                if (kvLight.Value.Reps > 0)
                {   
                    i++;
                    MarkerList +=("    ID: " + kvLight.Value.Id + " - " + kvLight.Value.Reps+ "\n");   
                }    
            }

            Console.ForegroundColor = ConsoleColor.Cyan;
            Console.WriteLine("-----------------------------------\n" +
                              "Ilosc kontrolek w uzyciu: "+i+ "" +
                              "\n-----------------------------------\n");
            Console.ResetColor();
            MarkerList += ("Wolne numery: \n");

            i=1;
            foreach (var kvLight in lights)
            {
                if (kvLight.Value.Reps <= 0)
                {   
                    i++;
                    MarkerList += ("    ID: " + kvLight.Value.Id+"\n");  
                }
            }
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("-----------------------------------\nIlosc nieuzywanych kontrolek: " + i +
                              "\n-----------------------------------\n");
            Console.ResetColor();

            CreateNewFile("C:/Users/praktykant/Desktop/Lista kontrolek.txt", MarkerList);
            
        }
        public void CreateNewFile(string path, string text)
        {
            File.WriteAllText(path, text);
        }


    }
}