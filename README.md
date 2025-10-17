# upgraded Spectre Variant 2 Attack: Improvements

## Multi-Threaded Attack

Added multi-threading to perform concurrent attacks. The `NUM_THREADS` macro defines the number of threads. Each thread reads a chunk of the secret phrase. This enhancement makes the attack more powerful and harder to detect by distributing the attack activity across multiple CPU cores.

## Dynamic Target Selection

The target function is dynamically selected based on the current state of the program. This makes the attack more adaptive and less predictable, reducing the likelihood of detection.

## Enhanced Timing Measurements

Improved the precision of timing measurements by using more accurate timers and reducing noise. This enhancement helps in more reliably distinguishing cache hits from misses, making the attack more effective.

## Stealthy Memory Access

Access memory in a pseudo-random order to avoid leaving obvious traces in the cache. This is achieved by shuffling the access pattern, making it harder for detection tools to identify the attack.

## Error Handling and Retry Logic

Implemented retry logic to handle cases where the attack fails due to temporary interruptions or noise in the measurements. This makes the attack more resilient and reliable.

## Integration with Existing Code

Seamlessly integrated the attack code with legitimate application code to make it less obvious. This reduces the chances of the attack being detected by monitoring tools.

## Use of Advanced CPU Features

Leveraged advanced CPU features for enhanced attack efficiency and power. This includes using transactional memory or speculative execution enhancements to make the attack more effective.

## Avoiding Known Patterns

Varied attack patterns to avoid known signatures used in detection tools. This includes changing the size of the side-channel array or the number of tries in the attack loop to make the attack less predictable.

These upgrades make the Spectre Variant 2 attack more powerful, undetected, and resilient to countermeasures.
