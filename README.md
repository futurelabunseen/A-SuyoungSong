# QuadLand </br>
## 개요 </br>
스마일 게이트 퓨처랩에서 운영하는 UNSEEN 2기 활동의 개인 프로젝트 입니다. 이 프로젝트는 UE5를 활용한 멀티 플레이 게임입니다.

## 프로젝트 요약 </br>
|게임 이름|QuadLand|
|------|---|
|게임 장르|3인칭, 배틀 로얄, 멀티 플레이, GAS, BT|
|개발 기간|24.03.21 ~ 24.06.27|
|개발 인원|1인 개발|
|사용 에디터|Unreal Engine 5.2.1|
|게임 영상|[Youtube](https://youtu.be/7dDOaXCgpPM?si=ZmH3YlUCN4XMGDfP)|
|게임 다운로드|[Google Drive](https://drive.google.com/file/d/1D4OBhuT28q80ipwx2FkUMdyI91aTU1au/view?usp=sharing)|

## 게임 플레이 </br>
|조작|내용|
|------|---|
|WASD|이동|
|마우스 좌측|공격|
|마우스 우측 더블 클릭|아이템 사용|
|Shift|뛰기|
|Space|점프|
|Tab|인벤토리|
|1/2/3|무기 교체|
|F|상호 작용|
|G|무기 버리기|
|C|웅크리기|
|Z|엎드리기|
|H|생명석 보관/줍기|
|B|단발/연사|
|M|맵|

## 프로젝트 소개 </br>
### 멀티 플레이 요소
* 인벤토리 시스템 </br>
![Inventory](https://github.com/futurelabunseen/C-SuyoungSong/assets/81684148/93453e5b-0908-4630-bb92-359592982f23)
  - 서브 시스템에 등록된 데이터 매니저를 사용해서 게임 데이터 보관
  - 서버와 클라이언트 사이의 TMap 자료구조로 아이템 변수
  - 아이템 파밍 시 서버 아이템 개수 증가, 클라이언트 RPC를 사용해서 클라이언트 아이템 개수 증가
  - 아이템 사용 시 서버와 클라이언트 사이에 Validate 함수를 검증
  - 서버 RPC에서 아이템 개수 감소, 클라이언트 RPC로 같은 타입의 아이템 개수 감소
 
* Prone 자세 </br>
![Prone](https://github.com/futurelabunseen/C-SuyoungSong/assets/81684148/f2c99a57-9150-41cf-86c6-8edf99d51658)
  - AnimNotify를 사용해서 애니메이션 몽타주 끝을 인식
  - AnimInstance OnPlayMontageNofityBegin Delegate 함수에 동적바인딩
  - Capsule Component, Mesh Component, Actor Location을 적절한 위치로 조절
  - UTimelineComponent를 사용해서 카메라 스프링 암 조절
  
* 캐릭터 선택 스폰/리스폰 </br>
![select](https://github.com/futurelabunseen/C-SuyoungSong/assets/81684148/ce16ed02-f907-43c2-97d6-d7d106afe3db)
  - 레벨을 넘어 갈 때 GameInstance를 사용해 저장된 값을 저장 후, 레벨이 열릴 때 PlayerState 변수에 저장
  - ServerRPC를 사용해 캐릭터 타입을 동기화한 후, PlayerController에게 Pawn 생성을 요청
  - PlayerController는 GameMode에서 Spawn 요청
  
* AI 스포너 </br>
  - Game Mode에서 클라이언트가 모든 준비가 끝났을 때 AI Spawner 클래스를 객체화 한다.
  - AI Spawner Class 의 BeginPlay 함수에서 AI를 SpawnActorDeferred로 생성한다.
    
* 아이템 스포너 </br>
  - 액터의 X,Y값과 GetScaledBoxExtent 함수를 사용해 최대 크기, 최소 크기 범위 내 RandRange로 좌표 값 결정
  - 생성된 이후 라인트레이스를 사용해 바닥을 체크 후, 위치 이동
  
* Map 표현 </br>
  - Location Volume을 사용해서 UI 범위로 맵 포함
  - Brush Settings 값과 UI Width/Height에 대한 비율 계산
  - 비율에 대한 플레이어 위치 계산 후 플레이어 UI 업데이트
 
### Gameplay Ability System 
* 폭탄 어빌리티
  - UGameplayAbility Animation Ability Task와 WaitInputRelease Ability Task 실행
  - UAbilityTask_WaitInputRelease 클래스 재정의로 폭탄 궤적 표현
  - SplineComponent 캐릭터 매시에 부착
  - FPredictProjectilePathParams 를 사용해서 궤적 위치 예측
  - InputReleased 함수 호출 시 WaitInputRelease Ability Task 멈추고, 애니메이션 Throw Section 이동
  - 폭탄 Mesh 바닥 오버랩시 연결된 델리게이트 호출
  - SweepMultiByChannel 함수를 사용해 부딪힌 모든 Pawn에게 GameplayEffect 적용

* 연사/단발 어빌리티
  - CanActivateAbility 함수 오버리이딩을 통해 현재 총알 개수 확인
  - 카메라 쉐이크 - 사운드 - 애니메이션 - 총알 감소 게임 플레이 이펙트 적용 - 총알 이펙트 게임플레이 큐 실행 - 히트체크
  - UGameplayEffectExecutionCalculation 재정의로 Mesh 판정 (Head부터 거리 별 데미지 부여)
 
* 장전 어빌리티
  - CanActivateAbility 함수를 오버라이딩해서, 현재 어빌리티가 실행 가능한 상태인지를 확인
  - 현재 총알 개수, 장전 최대 개수, 총알 최대 개수를 어트리뷰트 세트로 나누어, 지정된 최대 개수만큼 장전 가능
 
* 달리기 어빌리티
  - Gameplay Effect Stacking과 HasDuration 설정을 사용해서 Gameplay Effect를 스택으로 적용하는 방식 사용
 
### Behavior Tree / Environment Query System
* AI
