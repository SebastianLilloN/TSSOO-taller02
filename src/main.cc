#include <global.h>
#include <checkArgs.hpp>

uint64_t *g_numerosParalelos = nullptr;
uint64_t *g_numerosSerie = nullptr;
uint32_t sumaParalelo = 0;
uint32_t sumaSerial = 0;

void llenarArreglo(size_t start, size_t end, uint32_t limInf, uint32_t limSup)
{
	std::srand(std::time(0));
	std::random_device device;
	std::mt19937 rng(device());
	std::uniform_int_distribution<> unif(limInf, limSup);
	for (size_t i = start; i < end; ++i)
	{
		uint32_t randNumber = unif(rng);
		g_numerosParalelos[i] = randNumber;
	}
}

void sumaParcial(size_t limInf, size_t limSup)
{
	for (size_t i = limInf; i < limSup; ++i)
	{
		sumaParalelo += g_numerosParalelos[i];
	}
}

int main(int argc, char **argv)
{
	uint64_t totalElementos;
	uint32_t numThreads;
	uint32_t numLimInf;
	uint32_t numLimSup;
	std::vector<std::thread *> threadsLlenado;
	std::vector<std::thread *> threadsSum;
	auto argumentos = (std::shared_ptr<checkArgs>)new checkArgs(argc, argv);
	totalElementos = argumentos->getArgs().tamArr;
	numThreads = argumentos->getArgs().numThreads;
	numLimInf = argumentos->getArgs().limInf;
	numLimSup = argumentos->getArgs().limSup;

	g_numerosParalelos = new uint64_t[totalElementos];

	for (size_t i = 0; i < numThreads; ++i)
	{
		threadsLlenado.push_back(new std::thread((std::ref(llenarArreglo)),
											  i * (totalElementos) / numThreads,
											  (i + 1) * (totalElementos / numThreads), numLimInf, numLimSup));
	}

	for (size_t i = 0; i < numThreads; ++i)
	{
		threadsSum.push_back(new std::thread(std::ref(sumaParcial),
											 i * (totalElementos) / numThreads,
											 (i + 1) * (totalElementos / numThreads)));
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (auto &thS : threadsSum)
	{
		thS->join();
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoTotalSumaParalelo = elapsed.count();

	start = std::chrono::high_resolution_clock::now();
	for (auto &thF : threadsLlenado)
	{
		thF->join();
	}
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoTotalLlenadoParalelo = elapsed.count();

	g_numerosSerie = new uint64_t[totalElementos];

	start = std::chrono::high_resolution_clock::now();
	llenarArreglo(0, totalElementos, numLimInf, numLimSup);
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoTotalLlenadoSerial = elapsed.count();

	start = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < totalElementos; i++)
	{
		sumaSerial += g_numerosSerie[i];
	}
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoTotalSumaSerial = elapsed.count();

	std::cout << "Elementos                     : " << totalElementos << std::endl;
	std::cout << "Threads                       : " << numThreads << std::endl;
	std::cout << "Limite Inferior               : " << numLimInf << std::endl;
	std::cout << "Limite Superior               : " << numLimSup << std::endl;
	std::cout << "Suma en Paralelo              : " << sumaParalelo << std::endl;
	std::cout << "Suma en Serie                 : " << sumaParalelo << std::endl;
	std::cout << "Tiempo Total Llenado Serial   : " << tiempoTotalLlenadoSerial << " ms" << std::endl;
	std::cout << "Tiempo Total Suma Serial      : " << tiempoTotalSumaSerial << " ms" << std::endl;
	std::cout << "Tiempo Total Llenado Paralelo : " << tiempoTotalLlenadoParalelo << " ms" << std::endl;
	std::cout << "Tiempo Total Suma Paralela    : " << tiempoTotalSumaParalelo << " ms" << std::endl;
	std::cout << "Desempeño Serial              : " << (double)tiempoTotalLlenadoSerial/tiempoTotalLlenadoParalelo << " ms" << std::endl;
	std::cout << "Desempeño Paralelo            : " << (double)tiempoTotalSumaSerial/tiempoTotalSumaParalelo << std::endl;
	return (EXIT_SUCCESS);
}