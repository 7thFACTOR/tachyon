#include "types.h"

namespace ac
{
void Asset::recacheAbsolutePaths(BuildConfig* cfg)
{
    absoluteName = mergePathName(bundle->absolutePath, name);
    absolutePath = getFilenamePath(absoluteName);
    String bundleAndPath = mergePathPath(bundle->path, getFilenamePath(name));
    String bundlePathAndName = mergePathName(bundleAndPath, toString((u64)resId));
    absoluteOutputFilename = mergePathName(cfg->absoluteOutputPath, bundlePathAndName);
}

}