
current_user=$(whoami)
user_source_path=""
user_destination_path=""
source_dir="include"
destination_dir="build/include/ns3"

if [ "${current_user}" == "aniket" ]; then
    user_source_path="/home/${current_user}/D_drive/Study/MTP/ns-allinone-3.36.1/ns-3.36.1/scratch/mtp/"
    user_destination_path="/home/${current_user}/D_drive/Study/MTP/ns-allinone-3.36.1/ns-3.36.1/"
elif [ "${current_user}" == "ashu3103" ]; then
    user_source_path="/home/${current_user}/ns-allinone-3.36.1/ns-3.36.1/scratch/mtp/"
    user_destination_path="/home/${current_user}/ns-allinone-3.36.1/ns-3.36.1/"
fi


echo "Executing commands for user ${current_user}"

echo ""
echo "Executing git pull"
git pull

echo ""
echo "obtaining new files that were pulled."
files=()
for file in "${source_dir}/"*.h; do
    file_name=$(basename "${file}")
    files+=("$file_name")
    echo $file_name
done

echo ""
echo "Removing old symlink files"
for file in "${files[@]}"; do
echo "Removing ${destination_dir}/${file}"
rm -rf "${user_destination_path}${destination_dir}/${file}"
done

echo ""
echo "Creating new Symlinks"
for file in "${files[@]}"; do
    echo "Added ${source_dir}/${file} ${destination_dir}/${file}"
    ln -s "${user_source_path}${source_dir}/${file}" "${user_destination_path}${destination_dir}/${file}"
done

