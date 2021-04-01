#!/bin/bash
rm -rf ./lgdb
# pts='test'
pts='tuple_if2'
# pts='tuple_if2  logic  tuple_if  bits_rhs  firrtl_tail3  firrtl_tail2 
#      out_ssa  nested_if  counter  counter_nested_if 
#      adder_stage  if2 if  if3_err 
#      nested_if_err  firrtl_tail  ssa_rhs  reg__q_pin '


LGSHELL=./bazel-bin/main/lgshell
LGCHECK=./inou/yosys/lgcheck

if [ ! -f $LGSHELL ]; then
    if [ -f ./main/lgshell ]; then
        LGSHELL=./main/lgshell
        echo "lgshell is in $(pwd)"
    else
        echo "ERROR: could not find lgshell binary in $(pwd)";
    fi
fi



echo ""
echo ""
echo ""
echo "===================================================="
echo "Pyrope Full Compilation (C++ Parser)"
echo "===================================================="


for pt in $pts
do
    if [ ! -f inou/cfg/tests/${pt}.prp ]; then
      echo "ERROR: could not find ${pt}.prp in /inou/cfg/tests"
      exit 1
    fi

    ln -s inou/cfg/tests/${pt}.prp;


    echo "----------------------------------------------------"
    echo "Pyrope -> LNAST-SSA Graphviz debug"
    echo "----------------------------------------------------"

    ${LGSHELL} "inou.pyrope files:inou/cfg/tests/${pt}.prp |> inou.lnast_tolg.dbg_lnast_ssa |> inou.graphviz.from"

    if [ -f ${pt}.lnast.dot ]; then
      echo "Successfully create a lnast from inou/cfg/tests/${pt}.prp"
    else
      echo "ERROR: Pyrope compiler failed: LNAST generation, testcase: ${pt}.prp"
      exit 1
    fi

    echo "----------------------------------------------------"
    echo "Pyrope -> LNAST -> LGraph"
    echo "----------------------------------------------------"

    # ${LGSHELL} "inou.lnast_tolg.tolg files:${pt}.cfg"
    ${LGSHELL} "inou.pyrope files:inou/cfg/tests/${pt}.prp |> inou.lnast_tolg.tolg"
    if [ $? -eq 0 ]; then
      echo "Successfully create the inital LGraph: inou/cfg/tests/${pt}.prp"
    else
      echo "ERROR: Pyrope compiler failed: LNAST -> LGraph, testcase: inou/cfg/tests/${pt}.prp"
      exit 1

    fi


    ${LGSHELL} "lgraph.open name:${pt} |> inou.graphviz.from verbose:false"
    mv ${pt}.dot ${pt}.no_cprop.dot



#     echo ""
#     echo ""
#     echo ""
#     echo "----------------------------------------------------"
#     echo "Tuple Chain Resolve(LGraph)"
#     echo "----------------------------------------------------"
#     ${LGSHELL} "lgraph.open name:${pt} |> inou.lnast_tolg.resolve_tuples"
#     # ${LGSHELL} "lgraph.open name:${pt} |> pass.cprop"
#     if [ $? -eq 0 ]; then
#       echo "Successfully resolve the tuple chain: inou/cfg/tests/${pt}.prp"
#     else
#       echo "ERROR: Pyrope compiler failed: resolve tuples, testcase: inou/cfg/tests/${pt}.prp"
#       exit 1
#     fi

#     ${LGSHELL} "lgraph.open name:${pt} |> inou.graphviz.from verbose:false"
#     mv ${pt}.dot ${pt}.no_crop.dot


    echo ""
    echo ""
    echo ""
    echo "----------------------------------------------------"
    echo "Copy Propagation Optimization(LGraph)"
    echo "----------------------------------------------------"
    ${LGSHELL} "lgraph.open name:${pt} |> pass.cprop"
    if [ $? -eq 0 ]; then
      echo "Successfully eliminate all assignment or_op: inou/cfg/tests/${pt}.prp"
    else
      echo "ERROR: Pyrope compiler failed: cprop, testcase: inou/cfg/tests/${pt}.prp"
      exit 1
    fi

    ${LGSHELL} "lgraph.open name:${pt} |> inou.graphviz.from verbose:false"
    mv ${pt}.dot ${pt}.no_bw.dot


    echo ""
    echo ""
    echo ""
    echo "----------------------------------------------------"
    echo "Bitwidth Optimization(LGraph) Round-1"
    echo "----------------------------------------------------"

    ${LGSHELL} "lgraph.open name:${pt} |> pass.bitwidth"
    if [ $? -eq 0 ]; then
      echo "Successfully optimize design bitwidth: inou/cfg/tests/${pt}.prp"
    else
      echo "ERROR: Pyrope compiler failed: bitwidth optimization, testcase: inou/cfg/tests/${pt}.prp"
      exit 1
    fi

    ${LGSHELL} "lgraph.open name:${pt} |> inou.graphviz.from verbose:false"
    mv ${pt}.dot ${pt}.dot


    # echo ""
    # echo ""
    # echo ""
    # echo "----------------------------------------------------"
    # echo "Bitwidth Optimization(LGraph) Round-1"
    # echo "----------------------------------------------------"

    # ${LGSHELL} "lgraph.open name:${pt} |> pass.bitwidth"
    # if [ $? -eq 0 ]; then
    #   echo "Successfully optimize design bitwidth: inou/cfg/tests/${pt}.prp"
    # else
    #   echo "ERROR: Pyrope compiler failed: bitwidth optimization, testcase: inou/cfg/tests/${pt}.prp"
    #   exit 1
    # fi

    # ${LGSHELL} "lgraph.open name:${pt} |> inou.graphviz.from verbose:false"
    # mv ${pt}.dot ${pt}.assignment_or.dot


    # echo ""
    # echo ""
    # echo ""
    # echo "----------------------------------------------------"
    # echo "Assignment_Or_Op Elimination(LGraph)"
    # echo "----------------------------------------------------"
    # ${LGSHELL} "lgraph.open name:${pt} |> inou.lnast_tolg.assignment_or_elimination"
    # if [ $? -eq 0 ]; then
    #   echo "Successfully eliminate all assignment or_op: inou/cfg/tests/${pt}.prp"
    # else
    #   echo "ERROR: Pyrope compiler failed: assignment_or_elimination, testcase: inou/cfg/tests/${pt}.prp"
    #   exit 1
    # fi

    # ${LGSHELL} "lgraph.open name:${pt} |> inou.graphviz.from verbose:false"
    # mv ${pt}.dot ${pt}.cprop.dot

    # echo ""
    # echo ""
    # echo ""
    # echo "----------------------------------------------------"
    # echo "Copy Propagation Optimization(LGraph)"
    # echo "----------------------------------------------------"
    # ${LGSHELL} "lgraph.open name:${pt} |> pass.cprop"
    # if [ $? -eq 0 ]; then
    #   echo "Successfully eliminate all assignment or_op: inou/cfg/tests/${pt}.prp"
    # else
    #   echo "ERROR: Pyrope compiler failed: cprop, testcase: inou/cfg/tests/${pt}.prp"
    #   exit 1
    # fi

    # ${LGSHELL} "lgraph.open name:${pt} |> inou.graphviz.from verbose:false"
    # mv ${pt}.dot ${pt}.dce.dot

    # echo ""
    # echo ""
    # echo ""
    # echo "----------------------------------------------------"
    # echo "Dead Code Elimination(LGraph)"
    # echo "----------------------------------------------------"
    # ${LGSHELL} "lgraph.open name:${pt} |> inou.lnast_tolg.dce"
    # if [ $? -eq 0 ]; then
    #   echo "Successfully perform dead code elimination: inou/cfg/tests/${pt}.prp"
    # else
    #   echo "ERROR: Pyrope compiler failed: dead code elimination, testcase: inou/cfg/tests/${pt}.prp"
    #   exit 1
    # fi

    # ${LGSHELL} "lgraph.open name:${pt} |> inou.graphviz.from verbose:false"





    if [[ ${pt} == *_err* ]]; then
        echo "----------------------------------------------------"
        echo "Pass! This is a Compile Error Test, No Need to Generate Verilog Code "
        echo "----------------------------------------------------"
    else
        echo ""
        echo ""
        echo ""
        echo "----------------------------------------------------"
        echo "LGraph -> Verilog"
        echo "----------------------------------------------------"

        ${LGSHELL} "lgraph.open name:${pt} |> inou.yosys.fromlg"
        if [ $? -eq 0 ] && [ -f ${pt}.v ]; then
          echo "Successfully generate Verilog: ${pt}.v"
          rm -f  yosys_script.*
        else
          echo "ERROR: Pyrope compiler failed: verilog generation, testcase: inou/cfg/tests/${pt}.prp"
          exit 1
        fi


        echo ""
        echo ""
        echo ""
        echo "----------------------------------------------------"
        echo "Logic Equivalence Check"
        echo "----------------------------------------------------"

        ${LGCHECK} --implementation=${pt}.v --reference=./inou/cfg/tests/verilog_gld/${pt}.gld.v

        if [ $? -eq 0 ]; then
          echo "Successfully pass logic equivilence check!"
        else
          echo "FAIL: ${pt}.v !== ${pt}.gld.v"
          exit 1
        fi
    fi

    rm -f ${pt}.v
    rm -f ${pt}.prp
    rm -f lnast.dot.gld
    rm -f lnast.nodes
    rm -f lnast.nodes.gld
    # rm -f lnast.dot
    # rm -f *.dot
done #end of for
