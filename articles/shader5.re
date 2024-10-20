= ポストエフェクト

前章まででシェーダーの基礎から高度なテクニックまで学んできました。この章では、画面全体の演出を行うポストエフェクトについて学びます。これらの技術を習得することで、映像の質感をさらに高め、独自のビジュアル表現を実現できるようになります。

== ポストエフェクトで画面全体を演出

ポストエフェクト（ポストプロセスエフェクト）は、レンダリング後の画像に対して加工を施すことで、映像全体の雰囲気や質感を調整する手法です。モノクロやセピア色への変換、ワイプエフェクト、ブルーム効果など、様々な効果を実現できます。

=== モノクロやセピア色のポストエフェクト

このセクションでは、シーン全体をモノクロやセピア色に変換するポストエフェクトを実装します。これにより、映像にレトロな雰囲気や特定の感情を表現できます。

==== ポストプロセスパッケージのインストール

1. パッケージマネージャーの起動：
    - Unityエディタのメニューから、`Window > Package Manager`を選択します。
2. Post Processingパッケージのインストール：
    - パッケージリストから`Post Processing`を選択し、`Install`ボタンをクリックします。

==== ポストプロセスボリュームの設定

1. Post-processing Volumeオブジェクトの作成：
    - シーン内に空のゲームオブジェクトを作成し、名前を`PostProcessing`とします。
    - `PostProcessing`オブジェクトに`Post-process Volume`コンポーネントを追加します。
2. グローバル設定の有効化：
    - `Is Global`をチェックして、シーン全体にエフェクトを適用します。

==== モノクロ効果の実装

1. Color Gradingエフェクトの追加：
    - `Post-process Volume`コンポーネント内で、`Add Effect > Unity > Color Grading`を選択します。
2. 設定の調整：
    - `Saturation`（彩度）のパラメータを`100`に設定します。これにより、シーン全体がモノクロになります。

==== セピア効果の実装

1. Color Filterの適用：
    - `Color Grading`の`Color Filter`パラメータを有効化します。
    - セピア色（例：R=1.0, G=0.8, B=0.6）のフィルターを設定します。
2. 微調整：
    - `Temperature`や`Tint`のパラメータを調整して、色味を好みに合わせます。

==== 動作確認

- シーンビューまたはゲームビューで、エフェクトが正しく適用されていることを確認します。
- `Active`チェックボックスでエフェクトのオン・オフを切り替えて効果を比較します。

=== ワイプエフェクトの実装

ワイプエフェクトは、画面が徐々に変化する演出で、シーンの切り替えや特定のイベントに合わせて使用されます。

==== カスタムポストエフェクトの作成

1. シェーダーの作成：
    - `Assets`フォルダで右クリックし、`Create > Shader > Image Effect Shader`を選択します。
    - シェーダー名を`WipeEffectShader`とします。

//listnum[hlsl][WipeEffectShader]{
Shader "Hidden/WipeEffectShader"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
        _WipeAmount ("Wipe Amount", Range(0,1)) = 0
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        LOD 100

        Pass
        {
            CGPROGRAM
            #pragma vertex vert_img
            #pragma fragment frag
            #include "UnityCG.cginc"

            sampler2D _MainTex;
            float _WipeAmount;

            fixed4 frag(v2f_img i) : SV_Target
            {
                float2 uv = i.uv;
                if (uv.x < _WipeAmount)
                {
                    discard;
                }
                fixed4 col = tex2D(_MainTex, uv);
                return col;
            }
            ENDCG
        }
    }
}
//}

2. マテリアルの作成：
    - シェーダーを使用して新しいマテリアルを作成し、名前を`WipeEffectMaterial`とします。

==== ポストエフェクトスクリプトの作成

1. スクリプトの作成：
    - `Assets`フォルダで右クリックし、`Create > C# Script`を選択します。
    - スクリプト名を`WipeEffect`とします。

//listnum[csharp][WipeEffect]{
using UnityEngine;

[ExecuteInEditMode]
public class WipeEffect : MonoBehaviour
{
    public Material effectMaterial;
    [Range(0, 1)]
    public float wipeAmount = 0.0f;

    void OnRenderImage(RenderTexture src, RenderTexture dest)
    {
        if (effectMaterial != null)
        {
            effectMaterial.SetFloat("_WipeAmount", wipeAmount);
            Graphics.Blit(src, dest, effectMaterial);
        }
        else
        {
            Graphics.Blit(src, dest);
        }
    }
}
//}

2. カメラへの適用：
    - メインカメラに`WipeEffect`スクリプトをアタッチします。
    - `Effect Material`に`WipeEffectMaterial`を割り当てます。

==== 動作確認

`wipeAmount`の値を徐々に変化させて、ワイプエフェクトが正しく機能することを確認します。スクリプト内で時間経過に応じて`wipeAmount`を変更することで、自動的にエフェクトを進行させることも可能です。

//listnum[csharp][WipeEffect Update]{
void Update()
{
    wipeAmount += Time.deltaTime * 0.1f;
    if (wipeAmount > 1.0f)
        wipeAmount = 0.0f;
}
//}

=== ライトブルームで光を強調

ブルーム効果は、明るい部分が光を放っているように見せるポストエフェクトです。シーンの華やかさや現実感を高めることができます。

==== ブルームエフェクトの適用

1. `Bloom`エフェクトの追加：
    - `Post-process Volume`コンポーネントで、`Add Effect > Unity > Bloom`を選択します。

2. パラメータの設定：
    - `Intensity`：ブルームの強度を調整します（例：1.5）。
    - `Threshold`：ブルームを適用する明るさの閾値を設定します（例：1.0）。
    - `Soft Knee`：閾値周辺の滑らかさを調整します（例：0.5）。

3. カラーブーストの追加（任意）：
    - `Color Grading`エフェクトで`Post-exposure`を調整し、明るさを強調します。

==== 動作確認

シーン内の明るいオブジェクトや光源が、ブルーム効果によって光り輝いて見えることを確認します。パラメータを調整して、好みの効果を得られるようにします。

=== 注意事項

ブルーム効果はパフォーマンスに影響を与える可能性があります。必要に応じて品質設定を調整してください。