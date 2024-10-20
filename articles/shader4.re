= さらに進んだシェーダーテクニック

前章までで、基本的なシェーダーの実装方法やテクニックを学びました。この章では、さらに高度なシェーダーテクニックに挑戦し、表現の幅を広げていきます。頂点シェーダーを使った形状の変化や、ライティングモデルの深掘り、デプスバッファやステンシルバッファを活用した特殊効果など、実践的な内容を取り上げます。

== 頂点シェーダーで形状を変化させる

頂点シェーダーは、オブジェクトの頂点情報を操作することで、形状を動的に変化させることができます。これにより、オブジェクトのアニメーションや変形など、様々な表現が可能になります。

=== アウトラインシェーダーの作成

アウトラインシェーダーは、オブジェクトの輪郭を強調することで、視認性を高めたり、コミック風の表現を実現します。頂点シェーダーを用いて、オブジェクトの形状を膨張させることでアウトラインを描画します。

==== 目標

- オブジェクトの輪郭に沿ってアウトラインを描画する。
- アウトラインの色や太さを調整できるようにする。

==== シェーダーの実装

1. 新しいシェーダーの作成：
    - `Assets`フォルダで右クリックし、`Create > Shader > Unlit Shader`を選択します。
    - シェーダー名を`OutlineShader`とします。
2. シェーダーコードの編集：

//listnum[hlsl][OutlineShader]{
Shader "Custom/OutlineShader"
{
    Properties
    {
        _Color ("Outline Color", Color) = (0,0,0,1)
        _OutlineWidth ("Outline Width", Float) = 0.01
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" "Queue"="Geometry-1" }
        Cull Front
        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #include "UnityCG.cginc"

            float4 _Color;
            float _OutlineWidth;

            struct appdata
            {
                float4 vertex : POSITION;
                float3 normal : NORMAL;
            };

            struct v2f
            {
                float4 pos : SV_POSITION;
            };

            v2f vert (appdata v)
            {
                v2f o;
                float3 norm = UnityObjectToWorldNormal(v.normal);
                float3 pos = UnityObjectToWorldPos(v.vertex);
                pos += norm * _OutlineWidth;
                o.pos = UnityWorldToClipPos(pos);
                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                return _Color;
            }
            ENDCG
        }
    }
    FallBack "Diffuse"
}
//}

==== コードの解説

- **Properties**：アウトラインの色`_Color`と太さ`_OutlineWidth`を設定します。
- **Cull Front**：フロントカリングを行い、裏面のみを描画します。
- **頂点シェーダー`vert`**：法線方向に頂点を押し出して、オブジェクトを膨張させます。`UnityObjectToWorldNormal`と`UnityObjectToWorldPos`を用いて、オブジェクト空間からワールド空間へ変換します。
- **フラグメントシェーダー`frag`**：アウトラインの色を返します。

==== マテリアルの作成と適用

1. マテリアルの作成：
    - `Assets`フォルダで右クリックし、`Create > Material`を選択します。
    - マテリアル名を`OutlineMaterial`とします。
2. シェーダーの割り当て：
    - マテリアルのインスペクターで、シェーダーを`Custom/OutlineShader`に設定します。
3. オブジェクトへの適用：
    - アウトラインを付けたいオブジェクトに、`OutlineMaterial`を適用します。
4. 設定の調整：
    - マテリアルの`Outline Color`や`Outline Width`を調整して、アウトラインの見た目を変更します。

==== 動作確認

- シーンビューでオブジェクトを確認し、アウトラインが表示されていることを確認します。
- アウトラインの太さや色を変更して、効果の違いを試してみてください。

=== 旗や水面のなびきの表現

頂点シェーダーで頂点の位置を時間に応じて変化させることで、旗や水面が風になびくようなアニメーションを実現できます。

==== 目標

- 頂点シェーダーを使って、オブジェクトの頂点を動的に変形する。
- サイン波を利用して、自然ななびきの動きを表現する。

==== シェーダーの実装

1. 新しいシェーダーの作成：
    - `Assets`フォルダで右クリックし、`Create > Shader > Standard Surface Shader`を選択します。
    - シェーダー名を`WavingShader`とします。
2. シェーダーコードの編集：

//listnum[hlsl][WavingShader]{
Shader "Custom/WavingShader"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
        _WaveAmplitude ("Wave Amplitude", Float) = 0.5
        _WaveFrequency ("Wave Frequency", Float) = 1.0
        _WaveSpeed ("Wave Speed", Float) = 1.0
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        CGPROGRAM
        #pragma surface surf Standard vertex:vert

        sampler2D _MainTex;
        float _WaveAmplitude;
        float _WaveFrequency;
        float _WaveSpeed;

        struct Input
        {
            float2 uv_MainTex;
        };

        void vert (inout appdata_full v)
        {
            float wave = sin(_WaveFrequency * v.vertex.x + _Time.y * _WaveSpeed) * _WaveAmplitude;
            v.vertex.y += wave;
        }

        void surf (Input IN, inout SurfaceOutputStandard o)
        {
            fixed4 c = tex2D(_MainTex, IN.uv_MainTex);
            o.Albedo = c.rgb;
            o.Alpha = c.a;
        }
        ENDCG
    }
    FallBack "Diffuse"
}
//}

==== コードの解説

- **Properties**：波の振幅、周波数、速度を調整するパラメータを定義します。
- **頂点シェーダー`vert`**：頂点の`x`座標と時間を使ってサイン波を計算し、その値を`y`座標に加算します。これにより、オブジェクトが波打つように変形します。
- **サーフェスシェーダー`surf`**：テクスチャを適用して、オブジェクトの見た目を設定します。

==== マテリアルの作成と適用

1. マテリアルの作成：
    - `Assets`フォルダで右クリックし、`Create > Material`を選択します。
    - マテリアル名を`WavingMaterial`とします。
2. シェーダーの割り当て：
    - マテリアルのインスペクターで、シェーダーを`Custom/WavingShader`に設定します。
3. オブジェクトへの適用：
    - 平面（Plane）や旗のメッシュに`WavingMaterial`を適用します。
4. パラメータの調整：
    - `Wave Amplitude`、`Wave Frequency`、`Wave Speed`を調整して、なびきの強さや速さを変更します。

==== 動作確認

- プレイモードでシーンを再生し、オブジェクトが波打つように動くことを確認します。
- パラメータを変更して、動きの違いを試してみてください。
= 第5章 さらに進んだシェーダーテクニック

前章までで、基本的なシェーダーの実装方法やテクニックを学びました。この章では、さらに高度なシェーダーテクニックに挑戦し、表現の幅を広げていきます。頂点シェーダーを使った形状の変化や、ライティングモデルの深掘り、デプスバッファやステンシルバッファを活用した特殊効果など、実践的な内容を取り上げます。

== 頂点シェーダーで形状を変化させる

頂点シェーダーは、オブジェクトの頂点情報を操作することで、形状を動的に変化させることができます。これにより、オブジェクトのアニメーションや変形など、様々な表現が可能になります。

=== アウトラインシェーダーの作成

アウトラインシェーダーは、オブジェクトの輪郭を強調することで、視認性を高めたり、コミック風の表現を実現します。頂点シェーダーを用いて、オブジェクトの形状を膨張させることでアウトラインを描画します。

==== 目標

- オブジェクトの輪郭に沿ってアウトラインを描画する。
- アウトラインの色や太さを調整できるようにする。

==== シェーダーの実装

1. 新しいシェーダーの作成：
    - `Assets`フォルダで右クリックし、`Create > Shader > Unlit Shader`を選択します。
    - シェーダー名を`OutlineShader`とします。
2. シェーダーコードの編集：

//listnum[hlsl][OutlineShader]{
Shader "Custom/OutlineShader"
{
    Properties
    {
        _Color ("Outline Color", Color) = (0,0,0,1)
        _OutlineWidth ("Outline Width", Float) = 0.01
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" "Queue"="Geometry-1" }
        Cull Front
        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #include "UnityCG.cginc"

            float4 _Color;
            float _OutlineWidth;

            struct appdata
            {
                float4 vertex : POSITION;
                float3 normal : NORMAL;
            };

            struct v2f
            {
                float4 pos : SV_POSITION;
            };

            v2f vert (appdata v)
            {
                v2f o;
                float3 norm = UnityObjectToWorldNormal(v.normal);
                float3 pos = UnityObjectToWorldPos(v.vertex);
                pos += norm * _OutlineWidth;
                o.pos = UnityWorldToClipPos(pos);
                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                return _Color;
            }
            ENDCG
        }
    }
    FallBack "Diffuse"
}
//}

==== コードの解説

- **Properties**：アウトラインの色`_Color`と太さ`_OutlineWidth`を設定します。
- **Cull Front**：フロントカリングを行い、裏面のみを描画します。
- **頂点シェーダー`vert`**：法線方向に頂点を押し出して、オブジェクトを膨張させます。`UnityObjectToWorldNormal`と`UnityObjectToWorldPos`を用いて、オブジェクト空間からワールド空間へ変換します。
- **フラグメントシェーダー`frag`**：アウトラインの色を返します。

==== マテリアルの作成と適用

1. マテリアルの作成：
    - `Assets`フォルダで右クリックし、`Create > Material`を選択します。
    - マテリアル名を`OutlineMaterial`とします。
2. シェーダーの割り当て：
    - マテリアルのインスペクターで、シェーダーを`Custom/OutlineShader`に設定します。
3. オブジェクトへの適用：
    - アウトラインを付けたいオブジェクトに、`OutlineMaterial`を適用します。
4. 設定の調整：
    - マテリアルの`Outline Color`や`Outline Width`を調整して、アウトラインの見た目を変更します。

==== 動作確認

- シーンビューでオブジェクトを確認し、アウトラインが表示されていることを確認します。
- アウトラインの太さや色を変更して、効果の違いを試してみてください。

=== 旗や水面のなびきの表現

頂点シェーダーで頂点の位置を時間に応じて変化させることで、旗や水面が風になびくようなアニメーションを実現できます。

==== 目標

- 頂点シェーダーを使って、オブジェクトの頂点を動的に変形する。
- サイン波を利用して、自然ななびきの動きを表現する。

==== シェーダーの実装

1. 新しいシェーダーの作成：
    - `Assets`フォルダで右クリックし、`Create > Shader > Standard Surface Shader`を選択します。
    - シェーダー名を`WavingShader`とします。
2. シェーダーコードの編集：

//listnum[hlsl][WavingShader]{
Shader "Custom/WavingShader"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
        _WaveAmplitude ("Wave Amplitude", Float) = 0.5
        _WaveFrequency ("Wave Frequency", Float) = 1.0
        _WaveSpeed ("Wave Speed", Float) = 1.0
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        CGPROGRAM
        #pragma surface surf Standard vertex:vert

        sampler2D _MainTex;
        float _WaveAmplitude;
        float _WaveFrequency;
        float _WaveSpeed;

        struct Input
        {
            float2 uv_MainTex;
        };

        void vert (inout appdata_full v)
        {
            float wave = sin(_WaveFrequency * v.vertex.x + _Time.y * _WaveSpeed) * _WaveAmplitude;
            v.vertex.y += wave;
        }

        void surf (Input IN, inout SurfaceOutputStandard o)
        {
            fixed4 c = tex2D(_MainTex, IN.uv_MainTex);
            o.Albedo = c.rgb;
            o.Alpha = c.a;
        }
        ENDCG
    }
    FallBack "Diffuse"
}
//}

==== コードの解説

- **Properties**：波の振幅、周波数、速度を調整するパラメータを定義します。
- **頂点シェーダー`vert`**：頂点の`x`座標と時間を使ってサイン波を計算し、その値を`y`座標に加算します。これにより、オブジェクトが波打つように変形します。
- **サーフェスシェーダー`surf`**：テクスチャを適用して、オブジェクトの見た目を設定します。

==== マテリアルの作成と適用

1. マテリアルの作成：
    - `Assets`フォルダで右クリックし、`Create > Material`を選択します。
    - マテリアル名を`WavingMaterial`とします。
2. シェーダーの割り当て：
    - マテリアルのインスペクターで、シェーダーを`Custom/WavingShader`に設定します。
3. オブジェクトへの適用：
    - 平面（Plane）や旗のメッシュに`WavingMaterial`を適用します。
4. パラメータの調整：
    - `Wave Amplitude`、`Wave Frequency`、`Wave Speed`を調整して、なびきの強さや速さを変更します。

==== 動作確認

- プレイモードでシーンを再生し、オブジェクトが波打つように動くことを確認します。
- パラメータを変更して、動きの違いを試してみてください。

=== ポリゴンを点で表現するシェーダー

オブジェクトを点群（ポイントクラウド）として描画することで、独特のビジュアル表現を実現します。

==== 目標

- オブジェクトの頂点を点として描画する。
- 頂点シェーダーとジオメトリシェーダーを用いて点を生成する。

==== シェーダーの実装

**注意**：ジオメトリシェーダーは、一部のプラットフォーム
