= カオスとフラクタル

これまでの章で、シェーダーを用いた様々なグラフィックス表現を学んできました。この章では、一歩進んでカオスとフラクタルの世界に足を踏み入れます。これらは数学的な概念ですが、視覚的にも非常に興味深く、シェーダープログラミングと相性が良い分野です。

まずは有名なフラクタルであるマンデルブロ集合や、芸術との関わりについて探ります。その後、フラクタル研究で知られる宮田一乘（Miyata Kazunori）教授の研究を紹介し、それを踏まえてUnityでフラクタルを描画する方法を学びます。

== フラクタルの世界へ

=== フラクタルとは何か

フラクタルとは、部分と全体が自己相似性を持つ幾何学的な構造のことです。つまり、全体の形状を拡大していくと、その部分も全体と同じ形状を持っています。この性質は**自己相似性**と呼ばれます。

フラクタルは、自然界にも多く存在します。例えば、木の枝分かれのパターンや、海岸線の形状、雪の結晶などがフラクタル的な性質を持っています。

=== 有名なフラクタル：マンデルブロ集合とジュリア集合

**マンデルブロ集合**は、複素平面上で定義されるフラクタルの一種で、以下の漸化式で生成されます。

$$ z_{n+1} = z_n^2 + c $$

ここで、$z$は複素数で、$c$は定数です。初期値を$z_0 = 0$とし、無限に繰り返します。$|z_n|$が無限大に発散しない点の集合がマンデルブロ集合となります。

**ジュリア集合**も同様の漸化式で定義されますが、$c$を固定し、$z_0$を平面上の各点として計算します。

==== マンデルブロ集合の特徴

- 無限の複雑さ：どれだけ拡大しても新たな細部が現れます。
- 自己相似性：部分的なパターンが全体の形状と似ています。
- 美しい形状：視覚的に非常に魅力的で、多くの芸術作品の題材となっています。

== フラクタルとアート

=== フラクタルと芸術の関係

フラクタルは、その美しさと複雑さから、芸術作品にも多く取り入れられています。コンピュータを用いたフラクタルアートは、計算によって生成されるパターンを活用し、独特のビジュアルを生み出します。

=== ゴッホの作品とフラクタル

**フィンセント・ファン・ゴッホ**の絵画には、フラクタル的なパターンが含まれていると指摘する研究があります。特に、「星月夜」などの作品に見られる渦巻きやブラシストロークは、流体力学的なフラクタル性を示していると言われています。

==== 研究の概要

- 渦巻き模様：ゴッホの絵画に見られる渦巻きは、乱流のフラクタル特性と類似しています。
- フラクタル次元の分析：絵画のパターンを数値的に分析し、フラクタル次元を計算することで、その複雑さを評価します。

== 宮田一乘教授のフラクタル研究

=== 宮田一乘教授の紹介

*宮田一乘（Miyata Kazunori）**教授は、コンピュータグラフィックスの分野で活躍されている研究者で、特にフラクタルや自然現象のモデリングに関する研究で知られています。

=== フラクタルに関する研究

宮田教授の研究は、自然界の複雑な形状やパターンをコンピュータグラフィックスで再現することを目的としています。その中で、フラクタルを用いたモデリング手法を提案し、リアルな自然景観の生成に貢献しています。

==== 主な研究内容

- 自然物のモデリング：木々や山脈、雲など、フラクタル構造を持つ自然物を効率的に生成する手法の開発。
- フラクタルノイズの応用：パーリンノイズやシンプレックスノイズなどのフラクタルノイズを用いて、自然な質感やパターンを生成。
- リアルタイムレンダリング：GPUを活用したリアルタイムなフラクタル生成と描画技術の研究。

== Unityでフラクタルを描く

それでは、これまでの知識を踏まえて、Unityでフラクタルを描画してみましょう。具体的には、シェーダーを用いてマンデルブロ集合やフラクタルノイズを実装します。

=== マンデルブロ集合のシェーダー実装

==== 目標

- シェーダーでマンデルブロ集合を描画する。
- ズームや色彩の調整を行えるようにする。

==== シェーダーの実装手順

1. 新しいシェーダーの作成：
    - `Assets`フォルダで右クリックし、`Create > Shader > Unlit Shader`を選択します。
    - シェーダー名を`MandelbrotShader`とします。
2. シェーダーコードの編集：

//listnum[hlsl][MandelbrotShader]{
Shader "Custom/MandelbrotShader"
{
    Properties
    {
        _Iterations ("Iterations", Range(1, 1000)) = 100
        _Zoom ("Zoom", Float) = 1.0
        _Offset ("Offset", Vector) = (0,0,0,0)
        _Color ("Color", Color) = (1,1,1,1)
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #include "UnityCG.cginc"

            float _Iterations;
            float _Zoom;
            float2 _Offset;
            float4 _Color;

            struct appdata
            {
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
            };

            struct v2f
            {
                float4 pos : SV_POSITION;
                float2 uv : TEXCOORD0;
            };

            v2f vert (appdata v)
            {
                v2f o;
                o.pos = UnityObjectToClipPos(v.vertex);
                o.uv = v.uv;
                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                float2 c = (i.uv - 0.5) * _Zoom * 3.0 + _Offset;
                float2 z = float2(0.0, 0.0);
                int iter;
                for (iter = 0; iter < _Iterations; iter++)
                {
                    float x = (z.x * z.x - z.y * z.y) + c.x;
                    float y = (2.0 * z.x * z.y) + c.y;
                    if (x * x + y * y > 4.0)
                        break;
                    z = float2(x, y);
                }
                float t = iter / _Iterations;
                fixed4 color = lerp(float4(0, 0, 0, 1), _Color, t);
                return color;
            }
            ENDCG
        }
    }
}
//}

==== コードの解説

- **Properties**：
    - `_Iterations`：フラクタルの計算回数。多いほど詳細になるが、処理負荷が高くなる。
    - `_Zoom`：ズーム倍率。値を大きくすると拡大できる。
    - `_Offset`：表示位置のオフセット。拡大時の表示領域を調整する。
    - `_Color`：色の設定。
- **フラグメントシェーダー`frag`**：
    - `c`：複素数平面上の座標を計算。
    - `z`：漸化式の初期値（0, 0）。
    - `for`ループで漸化式を反復計算し、発散するまでの回数を測定。
    - `t`：発散

までの回数を正規化した値。
    - `lerp`関数で色を補間し、発散の速さに応じて色を変化。

==== マテリアルの作成と適用

1. マテリアルの作成：
    - `Assets`フォルダで右クリックし、`Create > Material`を選択します。
    - マテリアル名を`MandelbrotMaterial`とします。
2. シェーダーの割り当て：
    - マテリアルのインスペクターで、シェーダーを`Custom/MandelbrotShader`に設定します。
3. オブジェクトへの適用：
    - 平面（Quad）に`MandelbrotMaterial`を適用します。

==== 動作確認

- シーンビューやゲームビューで、マンデルブロ集合が描画されていることを確認します。
- マテリアルの`Iterations`、`Zoom`、`Offset`を調整して、拡大や表示位置の変更を行います。
- `Color`プロパティで色彩を変更し、好みのビジュアルに調整します。

=== フラクタルノイズの応用

**フラクタルノイズ**は、複数のノイズを重ね合わせて生成されるノイズで、自然な模様やテクスチャを作成するのに適しています。これをシェーダーで実装し、地形や雲などの質感表現に応用します。

==== フラクタルノイズの実装手順

1. 新しいシェーダーの作成：
    - シェーダー名を`FractalNoiseShader`とします。
2. シェーダーコードの編集：

//listnum[hlsl][FractalNoiseShader]{
Shader "Custom/FractalNoiseShader"
{
    Properties
    {
        _Octaves ("Octaves", Range(1, 8)) = 4
        _Persistence ("Persistence", Range(0, 1)) = 0.5
        _Scale ("Scale", Float) = 5.0
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #include "UnityCG.cginc"

            int _Octaves;
            float _Persistence;
            float _Scale;

            struct appdata
            {
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
            };

            struct v2f
            {
                float4 pos : SV_POSITION;
                float2 uv : TEXCOORD0;
            };

            // パーリンノイズ関数
            float Noise(float2 p)
            {
                return frac(sin(dot(p, float2(12.9898,78.233))) * 43758.5453);
            }

            // フラクタルノイズ関数
            float FractalNoise(float2 p)
            {
                float total = 0.0;
                float frequency = 1.0;
                float amplitude = 1.0;
                float maxValue = 0.0;
                for (int i = 0; i < _Octaves; i++)
                {
                    total += Noise(p * frequency) * amplitude;
                    maxValue += amplitude;
                    amplitude *= _Persistence;
                    frequency *= 2.0;
                }
                return total / maxValue;
            }

            v2f vert (appdata v)
            {
                v2f o;
                o.pos = UnityObjectToClipPos(v.vertex);
                o.uv = v.uv * _Scale;
                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                float n = FractalNoise(i.uv);
                return float4(n, n, n, 1);
            }
            ENDCG
        }
    }
}
//}

==== コードの解説

- **Properties**：
    - `_Octaves`：ノイズの重ね合わせ回数。多いほど複雑なノイズになる。
    - `_Persistence`：振幅の減衰率。値が小さいほど高周波成分が弱くなる。
    - `_Scale`：ノイズの拡大縮小。
- **Noise関数**：
    - 簡易的な疑似ノイズ関数を実装。
- **FractalNoise関数**：
    - ノイズを重ね合わせてフラクタルノイズを生成。

==== マテリアルの作成と適用

- マテリアル名を`FractalNoiseMaterial`とし、シェーダーを`Custom/FractalNoiseShader`に設定します。
- 平面（Quad）に適用し、ノイズのパターンを確認します。

==== 応用例

- ノイズの値を高さ情報として利用し、地形の生成に応用できます。
- カラーグラデーションを適用して、雲や炎のような質感を表現します。

=== カオスゲームとシェルピンスキーの三角形の実装

**カオスゲーム**は、単純なルールに従って点をプロットすることで、複雑なフラクタルパターンを生成する手法です。その代表的な例が**シェルピンスキーの三角形**です。

==== シェーダーの実装手順

1. 新しいシェーダーの作成：
    - シェーダー名を`ChaosGameShader`とします。
2. シェーダーコードの編集：

//listnum[hlsl][ChaosGameShader]{
Shader "Custom/ChaosGameShader"
{
    Properties
    {
        _Iterations ("Iterations", Range(1, 10000)) = 1000
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #include "UnityCG.cginc"

            int _Iterations;

            struct appdata
            {
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
            };

            struct v2f
            {
                float4 pos : SV_POSITION;
                float2 uv : TEXCOORD0;
            };

            v2f vert (appdata v)
            {
                v2f o;
                o.pos = UnityObjectToClipPos(v.vertex);
                o.uv = v.uv;
                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                float2 p0 = float2(0.5, 0.0);
                float2 p1 = float2(0.0, 1.0);
                float2 p2 = float2(1.0, 1.0);

                float2 p = float2(0.25, 0.5);
                for (int n = 0; n < _Iterations; n++)
                {
                    int r = int(frac(sin(float(n)) * 43758.5453) * 3);
                    if (r == 0) p = lerp(p, p0, 0.5);
                    if (r == 1) p = lerp(p, p1, 0.5);
                    if (r == 2) p = lerp(p, p2, 0.5);
                    if (distance(p, i.uv) < 0.001)
                    {
                        return float4(1, 1, 1, 1);
                    }
                }
                return float4(0, 0, 0, 1);
            }
            ENDCG
        }
    }
}
//}

==== コードの解説

- **カオスゲームのアルゴリズム**：
    - 初期点`p`を設定します。
    - 3つの頂点`p0`、`p1`、`p2`を定義します。
    - ランダムに頂点を選び、現在の点とその頂点の中点を新たな点とします。
    - この操作を繰り返し、点をプロットします。
- **フラグメントシェーダー`frag`**：
    - 各ピクセルについて、カオスゲームの計算を行い、そのピクセルが点の位置に含まれるかを判定します。
    - 該当する場合は白色、それ以外は黒色を返します。

==== マテリアルの作成と適用

- マテリアル名を`ChaosGameMaterial`とし、シェーダーを`Custom/ChaosGameShader`に設定します。
- 平面（Quad）に適用し、シェルピンスキーの三角形が描画されることを確認します。

==== 動作確認

- `Iterations`の値を増やすと、より詳細なパターンが生成されます。
- ランダム数のシード値や頂点の位置を変更して、異なるフラクタルパターンを試してみてください。

== まとめ

この章では、カオスとフラクタルの概念を学び、それをシ

ェーダーで実装する方法を紹介しました。フラクタルは数学的には複雑ですが、シンプルなアルゴリズムで美しいパターンを生成できる点が魅力です。

Unityのシェーダープログラミングを通じて、フラクタルの世界を探求することで、数学的な美しさとプログラミングの楽しさを感じていただけたでしょうか。これらの技術を応用して、さらに独自の表現を生み出してみてください。

次章では、シェーダーとレンダリングの最前線について学び、最新の技術動向や今後の展望を探ります。

