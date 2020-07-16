#!/usr/bin/env python3

import os, sys, shutil, git

def get_repository_directory() -> str:
	try:
		repository = git.Repo(path='.', search_parent_directories=True)
		directory: str = repository.git_dir
		return os.path.abspath(os.path.join(directory, os.path.pardir))
	except git.InvalidGitRepositoryError:
		return ''

def get_package_directories() -> list:
	platform: str = sys.platform
	if platform == 'linux':
		return ['installed/x64-linux']
	elif platform == 'darwin':
		return ['installed/x64-osx']
	elif platform == 'win32':
		return ['installed/x86-windows', 'installed/x64-windows']
	return []

def get_cmake_files() -> list:
	repository_directory: str = get_repository_directory()
	if len(repository_directory) == 0:
		print('Error! This script is not located in the \"leviathan-racket\" git repository!')
		return []
	cmake_directory: str = os.path.join(repository_directory, 'cmake')
	if not os.path.isdir(cmake_directory):
		print('Error! Cannot find valid cmake directory in repository!')
		return []
	cmake_files: list = [
		os.path.join(cmake_directory, 'TmxliteConfig.cmake'),
		os.path.join(cmake_directory, 'TmxliteConfigVersion.cmake'),
		os.path.join(cmake_directory, 'TmxliteTargets-debug.cmake'),
		os.path.join(cmake_directory, 'TmxliteTargets-release.cmake'),
		os.path.join(cmake_directory, 'TmxliteTargets.cmake')
	]
	error_exists: bool = False
	for file in cmake_files:
		if not os.path.isfile(file):
			error_exists = True
			print(f'Error! Cannot locate necessary file: {file}!')
	if error_exists:
		return []
	return cmake_files

def main():
	vcpkg_directory: str = os.path.join(os.environ.get('HOME'), 'vcpkg')
	vcpkg_was_input: bool = False
	if len(sys.argv) > 1:
		vcpkg_was_input = True
		vcpkg_directory = sys.argv[1]
	if not os.path.exists(vcpkg_directory):
		if vcpkg_was_input:
			print(f'Error! The path {vcpkg_directory} is an invalid path!')
		else:
			print('Error! Couldn\'t find vcpkg in home directory! Pass in the path to the vcpkg root directory!')
		return
	if not os.path.isdir(vcpkg_directory) and vcpkg_was_input:
		print(f'Error! The path {vcpkg_directory} does not refer to a directory!')
		return
	if not os.path.exists(os.path.join(vcpkg_directory, '.vcpkg-root')):
		print('Error! Your vcpkg installation isn\'t valid!')
		return
	package_directories: list = get_package_directories()
	if len(package_directories) == 0:
		print('Error! Unsupported operating system! You can only build for Windows, MacOS, or Linux!')
		return
	cmake_files = get_cmake_files()
	if len(cmake_files) == 0:
		print('Error! Couldn\'t find necessary tmxlite cmake files!')
		return
	for package_directory in package_directories:
		tmxlite_directory: str = os.path.join(vcpkg_directory, package_directory, 'share/tmxlite')
		if os.path.isdir(tmxlite_directory):
			for cmake_file in cmake_files:
				shutil.copy(cmake_file, tmxlite_directory)

if __name__ == '__main__':
	main()
