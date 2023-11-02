function PrintAssets($dir,$file) {
	ls "../hand/App/$dir/$file" | ?{ -not $_.PSIsContainer } | %{ "Resource($dir/$([string]$_.Name))" }
}

PrintAssets "font" "*.ttf"
PrintAssets "shader" "*.hlsl"
PrintAssets "texture" "*.png"
