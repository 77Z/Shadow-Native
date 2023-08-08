const sources = [];
for (let i = 0; i < 50; i++) {
	sources.push(`source${i}.cpp`);
}

// Original solution of grouping work into batches
// Each thread would be assigned a source to compile then when it was
// done it had to wait for the next batch, this led to threads waiting
// around for work when one thread would have to crunch a large file
// like Runtime.cpp which can take like 11 seconds to compile.
/* const batches: SrcDestCombo[][] = [];

while (sourcesToBuild.length > 0) {
    batches.push(sourcesToBuild.splice(0, navigator.hardwareConcurrency));
}

// PRINT("BATCHES:");
// PRINT(batches);

for (const batch of batches) {
    hasToLink = true;
    PRINT("Compiling batch " + batches.indexOf(batch));
    const promises: Promise<void>[] = [];
    for (const source of batch) {
        promises.push((async () => {
            // const artificialDelay = Math.floor(Math.random() * (7000 - 2000 + 1)) +
            //     2000;
            // await new Promise((resolve) => setTimeout(resolve, artificialDelay));
            // PRINT(`Compiled source ${source.source} in ${artificialDelay}ms`);

            const startTime = performance.now();
            await compile(target, source);
            console.log(
                `Compiled source ${source.source} in ${
                    performance.now() - startTime
                }ms`,
            );
        })());
    }

    await Promise.allSettled(promises);
} */

// First solution tried, breaks and pops off undefined items. Bad solution
/* const promises: Promise<void>[] = [];

for (let i = 0; i < navigator.hardwareConcurrency; i++) {
	promises.push((async () => {
		const localSources = [...sources];
		while (localSources.length > 0) {
			const start = performance.now();
			const artificialDelay = Math.floor(Math.random() * (7000 - 2000 + 1)) +
				2000;
			await new Promise((resolve) => setTimeout(resolve, artificialDelay));
			console.log(
				`Finished ${localSources.pop()} in ${performance.now() - start}ms`,
			);
		}
	})());
}
 */

// Chunks and divides work, imo no better than the batching we currently have
/*
const promises: Promise<void>[] = [];
const chunkSize = Math.ceil(sources.length / navigator.hardwareConcurrency);

for (let i = 0; i < navigator.hardwareConcurrency; i++) {
	const startIdx = i * chunkSize;
	const endIdx = startIdx + chunkSize;
	const chunk = sources.slice(startIdx, endIdx);

	promises.push(
		(async () => {
			for (const source of chunk) {
				const start = performance.now();
				const artificialDelay = Math.floor(Math.random() * (7000 - 2000 + 1)) +
					2000;
				await new Promise((resolve) => setTimeout(resolve, artificialDelay));
				console.log(`Finished ${source} in ${performance.now() - start}ms`);
			}

			console.log("Finished all my work: promise " + i);
		})(),
	);
} */

//Dynamic load balancing algorithm
const totalPromises = navigator.hardwareConcurrency;
const promises = new Array(totalPromises);

for (let i = 0; i < totalPromises; i++) {
	promises[i] = (async () => {
		let nextIndex = i;

		while (nextIndex < sources.length) {
			const source = sources[nextIndex];
			const start = performance.now();
			const artificialDelay = Math.floor(Math.random() * (3500 - 1000 + 1)) +
				1000;
			await new Promise((resolve) => setTimeout(resolve, artificialDelay));
			console.log(
				`Promise ${i} finished ${source} in ${performance.now() - start}ms`,
			);

			// Choose the next index based on the number of promises available
			nextIndex += totalPromises;
		}

		console.log("Finished my work! promise " + i);
	})();
}
