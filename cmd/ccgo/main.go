// Copyright 2022 gorse Project Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
package main

import (
	"fmt"
	"github.com/scylladb/go-set/strset"
	"github.com/spf13/cobra"
	"modernc.org/cc/v3"
	"os"
	"os/exec"
)

var supportedTypes = strset.New("int64_t")

type Function struct {
	Name       string
	Parameters []string
}

// printError prints an error message with its position.
func printError(node cc.Node, message string) {
	position := node.Position()
	_, _ = fmt.Fprintf(os.Stderr, "%v:%v:%v: error: %v\n", position.Filename, position.Line, position.Column, message)
	os.Exit(1)
}

// parseFunctions parse C source file and extract functions declarations.
func parseFunctions(fileName string) []Function {
	ast, err := cc.Parse(&cc.Config{}, nil, []string{"/usr/include", "/usr/local/lib/gcc/x86_64-pc-linux-gnu/12.1.1/include"}, []cc.Source{{Name: fileName}})
	if err != nil {
		_, _ = fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
	var functions []Function
	for _, nodes := range ast.Scope {
		if len(nodes) != 1 || nodes[0].Position().Filename != fileName {
			continue
		}
		node := nodes[0]
		if declarator, ok := node.(*cc.Declarator); ok {
			funcIdent := declarator.DirectDeclarator
			if funcIdent.Case != cc.DirectDeclaratorFuncParam {
				continue
			}
			functions = append(functions, convertFunction(funcIdent))
		}
	}
	return functions
}

// convertFunction extracts the function definition from cc.DirectDeclarator.
func convertFunction(declarator *cc.DirectDeclarator) Function {
	return Function{
		Name:       declarator.DirectDeclarator.Token.Value.String(),
		Parameters: convertFunctionParameters(declarator.ParameterTypeList.ParameterList),
	}
}

// convertFunctionParameters extracts parameters from cc.ParameterList.
func convertFunctionParameters(params *cc.ParameterList) []string {
	declaration := params.ParameterDeclaration
	paramName := declaration.Declarator.DirectDeclarator.Token.Value
	paramType := declaration.DeclarationSpecifiers.TypeSpecifier.Token.Value
	isPointer := declaration.Declarator.Pointer != nil
	if !isPointer && !supportedTypes.Has(paramType.String()) {
		printError(declaration, fmt.Sprintf("unsupported type: %v", paramType))
	}
	paramNames := []string{paramName.String()}
	if params.ParameterList != nil {
		paramNames = append(paramNames, convertFunctionParameters(params.ParameterList)...)
	}
	return paramNames
}

// runCommand runs a command and extract its output.
func runCommand(name string, arg ...string) string {
	cmd := exec.Command(name, arg...)
	output, err := cmd.CombinedOutput()
	if err != nil {
		if output != nil {
			_, _ = fmt.Fprintln(os.Stderr, string(output))
		} else {
			_, _ = fmt.Fprintln(os.Stderr, err)
		}
		os.Exit(1)
	}
	return string(output)
}

var command = &cobra.Command{
	Use: "ccgo",
	Run: func(cmd *cobra.Command, args []string) {
		sourceFileName := "/data/home/zhenghaoz/GolandProjects/gorse_bug/base/floats/src/floats_amd64.c"
		asmFileName := "/data/home/zhenghaoz/GolandProjects/gorse_bug/base/floats/src/floats_amd64.s"
		objectFileName := "/data/home/zhenghaoz/GolandProjects/gorse_bug/base/floats/src/floats_amd64.o"
		//goFileName := "/data/home/zhenghaoz/GolandProjects/gorse_bug/base/floats/floats_amd64.go"
		defer func() {
			_ = os.Remove(asmFileName)
			_ = os.Remove(objectFileName)
		}()
		// parse source
		functions := parseFunctions(sourceFileName)
		fmt.Println(functions)
		// compile to asm
		runCommand("clang", "-O3", "-S", "-mavx2", "-mfma", "-mno-red-zone", "-mstackrealign", "-mllvm",
			"-inline-threshold=1000", "-fno-asynchronous-unwind-tables", "-fno-exceptions", "-fno-rtti",
			"-c", sourceFileName,
			"-o", asmFileName)
		// compile to object
		runCommand("clang", "-O3", "-mavx2", "-mfma", "-mno-red-zone", "-mstackrealign", "-mllvm",
			"-inline-threshold=1000", "-fno-asynchronous-unwind-tables", "-fno-exceptions", "-fno-rtti",
			"-c", sourceFileName,
			"-o", objectFileName)
		// object dump
		dump := runCommand("objdump", "-d", objectFileName)
		fmt.Println(dump)
	},
}

func main() {
	if err := command.Execute(); err != nil {
		_, _ = fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
}
