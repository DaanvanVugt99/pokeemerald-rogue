using System;
using PokemonDataGenerator.Pokedex;

namespace PokemonDataGenerator
{
    internal static class ProfilePipelineProgram
    {
        private static int Main(string[] args)
        {
            if (args.Length < 1 || args.Length > 2
                || (args[0] != "refresh" && args[0] != "generate" && args[0] != "verify")
                || (args.Length == 2 && (args[0] != "refresh" || args[1] != "--accept-removals")))
            {
                Console.Error.WriteLine("Usage: ProfilePipeline.exe refresh [--accept-removals] | generate | verify");
                return 2;
            }
            try
            {
                PokemonProfilePipeline.Run(args[0], args.Length == 2);
                return 0;
            }
            catch (Exception exception)
            {
                Console.Error.WriteLine(exception);
                return 1;
            }
        }
    }
}
