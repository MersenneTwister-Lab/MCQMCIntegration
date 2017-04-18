モンテカルロ法および準モンテカルロ法による数値積分
=====================================

このプログラムはモンテカルロ法および準モンテカルロ法による数値積分を行う。

依存システム
===========
* C++11 または C++14
* LITTLE ENDIAN システム

ダウンロード
=========
* [mcqmcintegration-0.3.tar.gz](mcqmcintegration-0.3.tar.gz)

インストール
=========
インストール先をYOUR_INSTALL_DIRとすると、

```
./configure --prefix=YOUR_INSTALL_DIR
make
make install
```

準備
===
環境変数 SOBOL_PATH をインストールディレクトリ以下のshare/mcqmcintegration/に
セットする。

```{.sh}
export SOBOL_PATH=YOUR_INSTALL_DIR/share/mcqmcintegration/
```

LIBRARY_PATH と CPLUS_INCLUDE_PATH を適切にセットする。

注意
===
データファイル 'sobolbase.dat' はLittel Endianシステムで作られた
バイナリファイルであり、 Big Endian システムでは正確に動作しないだろう。


使用法
=====
準モンテカルロ法及びモンテカルロ法による数値積分の計算をするC++関数は以下のとおり。

- [MCQMCIntegration::quasi_monte_carlo_integration()](../namespace_m_c_q_m_c_integration.html#a87dcd147ab8d52d91879b9720d36f8d7)
- [MCQMCIntegration::monte_carlo_integration()](../namespace_m_c_q_m_c_integration.html#a84bc3922ec1ea308a567600ef22c5fc3)

準モンテカルロ法で使う超一様分布列の指定は以下の列挙型を使う。

- [MCQMCIntegration::DigitalNetID](../namespace_m_c_q_m_c_integration.html#ae16e0540efd559e5c38e8d397e00f13c)

返却値は次の構造体になる。

- [MCQMCIntegration::MCQMCResult](../struct_m_c_q_m_c_integration_1_1_m_c_q_m_c_result.html)

MCQMCIntegration::quasi_monte_carlo_integration()の引数
-----------------------------------------------------
<dl>
  <dt>uint32_t  	N</dt>
  <dd>積分を行う回数。この回数だけ積分を繰り返し、平均と誤差を求める。</dd>

  <dt>I &  	integrand </dt>
  <dd>I テンプレートパラメータ。被積分関数。ユーザー定義のクラス</dd>

  <dt>DigitalNetID	dnid</dt>
  <dd>使用するデジタルネットを表す。
  <ul>
  <li>SOBOL : 高次元(21201次元)まで定義された Sobol 点集合。</li>
  <li>NXLW  : WAFOM値の低い Niederreiter-Xing 点集合。</li>
  <li>SOLW  : WAFOM値の低い　Sobol 点集合。</li>
  </ul>
  </dd>

  <dt>uint32_t  	s</dt>
  <dd>積分領域の次元。</dd>

  <dt>uint32_t  	m</dt>
  <dd>デジタルネットの上位mビットまでの精度, また１回の積分で2^m個の点を使用する。</dd>

  <dt>int  	probability</dt>
  <dd>積分結果が誤差内に入ると期待される確率を表す整数。95, 99, 999, 9999のひとつ。</dd>
</dl>

MCQMCIntegration::monte_carlo_integration()の引数
------------------------------------------------
<dl>
  <dt>uint32_t  	s</dt>
  <dd>積分領域の次元。</dd>

  <dt>uint32_t  	m</dt>
  <dd>１回の積分で2^m個の点を使用する。</dd>

  <dt>uint32_t  	N</dt>
  <dd>積分を行う回数。この回数だけ積分を繰り返し、平均と誤差を求める。</dd>

  <dt>I &  	integrand </dt>
  <dd>I テンプレートパラメータ。被積分関数。ユーザー定義のクラス</dd>

  <dt>R &  	rand </dt>
  <dd>R テンプレートパラメータ。擬似乱数クラス, C++11のrandomで定義されたクラス</dd>

  <dt>D &  	dist </dt>
  <dd>D テンプレートパラメータ。擬似乱数の分布のクラス。
  C++11のrandomで定義された実数一様分布クラス</dd>

  <dt>int  	probability</dt>
  <dd>積分結果が誤差内に入ると期待される確率を表す整数。95, 99, 999, 9999のひとつ。</dd>
</dl>

準モンテカルロ法、およびモンテカルロ法による数値積分の返却値
---------------------------------------------
<dl>
  <dt>value</dt>
  <dd>数値積分の結果の平均値。</dd>

  <dt>error</dt>
  <dd>絶対誤差</dd>
</dl>

被積分関数の条件
------------
被積分関数クラスは以下の条件を満たす operator() 演算子を定義する必要がある。

- 返却値は double
- 引数は const double[] 一つ
- operator()が呼び出される時に引数として与えられる配列の長さは、積分領域の次元sである。

プログラム例
=========

- [準モンテカルロ法の例](qmc-example.html)
- [モンテカルロ法の例](mc-example.html)
- [準モンテカルロ法による高次元積分の例](qmc_largesobol_example.html)

参考文献
======
- [Sobol sequence generator](http://web.maths.unsw.edu.au/~fkuo/sobol/)
- [GitHub プロジェクト](https://github.com/MersenneTwister-Lab/MCQMCIntegration)
- [GitHub Web ページ](https://mersennetwister-lab.github.io/MCQMCIntegration/)

LICENSE
=======
````
    Copyright (c) 2008, Frances Y. Kuo and Stephen Joe.
    All rights reserved.

    Copyright (C) 2017 Shinsuke Mori, Makoto Matsutmoto, Mutsuo Saito
    and Hiroshima University.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
````