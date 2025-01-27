typedef struct OrbisLibcMallocManagedSize
{
  uint16_t size;
  uint16_t version;
  uint32_t reserved1;
  size_t maxSystemSize;
  size_t currentSystemSize;
  size_t maxInuseSize;
  size_t currentInuseSize;
} OrbisLibcMallocManagedSize;

typedef void *OrbisLibcMspace;

extern "C"
{
  void *sceLibcMspaceCalloc(OrbisLibcMspace msp, size_t nelem, size_t size);
  OrbisLibcMspace sceLibcMspaceCreate(const char *name, void *base, size_t capacity, uint32_t flag);
  int32_t sceLibcMspaceDestroy(OrbisLibcMspace msp);
  int32_t sceLibcMspaceFree(OrbisLibcMspace msp, void *ptr);
  int32_t sceLibcMspaceIsHeapEmpty(OrbisLibcMspace msp);
  void *sceLibcMspaceMalloc(OrbisLibcMspace msp, size_t size);
  int32_t sceLibcMspaceMallocStats(OrbisLibcMspace msp, OrbisLibcMallocManagedSize *mmsize);
  int32_t sceLibcMspaceMallocStatsFast(OrbisLibcMspace msp, OrbisLibcMallocManagedSize *mmsize);
  size_t sceLibcMspaceMallocUsableSize(void *ptr);
  void *sceLibcMspaceMemalign(OrbisLibcMspace msp, size_t boundary, size_t size);
  int32_t sceLibcMspacePosixMemalign(OrbisLibcMspace msp, void **ptr, size_t boundary, size_t size);
  void *sceLibcMspaceRealloc(OrbisLibcMspace msp, void *ptr, size_t size);
  void *sceLibcMspaceReallocalign(OrbisLibcMspace msp, void *ptr, size_t boundary, size_t size);
}