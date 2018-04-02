echo "Please wait..."
#if [ ! -d "out/target/product/cb04_6735_35u/obj/APPS/"]; then 
echo "copy..."
mkdir -p out/target/product/cb04_6735_35u/obj/
cp -r vendor/mediatek/prebuilt_simcom/target/product/cb04_6735_35u/obj/APPS/ out/target/product/cb04_6735_35u/obj/
cp -r vendor/mediatek/prebuilt_simcom/target/product/cb04_6735_35u/obj/SHARED_LIBRARIES/ out/target/product/cb04_6735_35u/obj/
cp -r vendor/mediatek/prebuilt_simcom/target/product/cb04_6735_35u/module/* out/target/product/cb04_6735_35u/
cp -r vendor/blink/* out/target/product/cb04_6735_35u/system/
#fi
echo "done"
