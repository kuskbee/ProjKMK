# 🐉 프로젝트 와이번테일

[![유튜브 영상](./docs/thumbnail.png)](https://youtu.be/blPDOXV9d7c?si=lnJIYtk3caMp4l9T)

**몬스터 헌터 스타일의 3인칭 액션 게임, 싱글·멀티 플레이 지원**
- (클릭하면 유튜브영상으로 이동합니다.)

---

## 프로젝트 소개
- **목표**: Monster Hunter 스타일의 액션 전투 체험 제공  
- **플랫폼**: Unreal Engine 5.5 (PC)  
- **모드**:  
  - 싱글플레이: AI 몬스터, 카메라 셰이크, 페이즈별 로직
  - 멀티플레이:
    - TCP 기반 로그인 / 데디케이티드 서버 통신
    - Player/Monster 동기화
    - GameState, GameMode 분리

---

## 주요 기능

### 1차 싱글 콘텐츠
- 게임플레이  
  - 몬스터 AI (패트롤, 추적→공격 전환, HitReact, Dead 처리)  
  - 플레이어 공격(콤보, 대시 어택, 애니메이션 블렌딩)  
  - 카메라 Shake, UI HP 바, 포탈 연출  
  - 락온(타겟팅 뷰)모드

---

### C++ 전환
- 핵심 클래스로 전환  
  - `GameMode`, `PlayerController`, `EchoCharacter → PlayerCharacter`  
  - `Weapon`, `TargetSystem (lock‐on)`, `DashAttack`, `Portal`  
- RPC & 멀티플레이용 리팩토링 진행 중  

---

### 2차 멀티플레이
- 네트워킹  
  - TCP 로그인 서버
      - 역할       : UE5 클라이언트 ↔ MySQL 인증 / UE5 Dedicated Server 연결 중계
    - 언어       : C++
    - DB         : MySQL 8.x  + Connector/C++ 8.x
    - Networking : WinSock (Blocking) — 1 스레드 당 1 클라
    - Protocol   : FlatBuffers (4B Length-Prefix)
  - Dedicated Server 환경 구성  

- 동기화 작업
  - 플레이어 위치/애니메이션/대미지 동기화  
  - 몬스터 AI 동기화 및 대미지 처리
  - 포탈 사용 동기화
  - 참여 플레이어 리스트 UI 추가


---

## 🛠 기술 스택
- **엔진**: Unreal Engine 5.5  
- **언어**: C++, Blueprints  
- **네트워크**: Winsock2 기반 TCP 서버, Dedicated 서버 
- **도구**: Git, Sourcetree, Notion, Discord  


## 👥 팀원 & 역할

| 팀원                         | 주요 역할 & 담당 영역                                                                                                                                          |
|----------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **구서경(kuskbee)**      | - **플레이어 캐릭터**: 공격 콤보·대시·애니메이션 블렌딩 및 C++ 전환 작업<br>- **포탈/제단 기믹** : 몬스터 꼬리를 이용한 포탈 오픈 기믹 및 멀티 동기화 작업<br>- **Dedicated Server** 환경 구축 및 운영<br>- **TCP 로그인 서버** ↔ MySQL 인증 중계 (Connector/C++ 사용)  <br>- **네트워크 클라이언트**: UE5 ↔ TCP 로그인 서버 통신<br>   - **프로토콜 연동**: FlatBuffers 메시징  <br>      |
| **문한준(nanumoon)**                | - **몬스터 AI 블루프린트 작업** : 패트롤·추적→공격 전환·HitReact·죽음 처리 <br> - **GameState / GameMode** 설계·구현         <br>- **멀티 동기화**: 플레이어 캐릭터 위치/애니메이션/데미지<br>   - **UI 멀티 작업**: 플레이어 HP 바,  Game Lose/Win UI 표시 <br>                |
| **오태형(dhxogud)**    | - **몬스터 AI C++ 전환 작업**<br>     - **몬스터 네트워킹 연동** : 몬스터 캐릭터의 스탯, 애니메이션, 상태 처리,<br> &nbsp;&nbsp;몬스터 캐릭터 AI (Behavior Tree, AIController)를 멀티 환경 최적화<br> &nbsp;&nbsp;몬스터와 플레이어 캐릭터의 멀티 환경을 위한 상호작용 작업 진행<br>             |
| **강채림(rim014)**    | - **리소스·레벨 디자인**: 인게임 자연 및 우주 컨셉 맵·에셋 구성<br>- **VFX & 이펙트**: 포탈, 제단 이펙트 및 검기, 피격 이펙트 제작<br>- **UI 리소스**: 몬스터 HP 및 Game Lose/Win UI 디자인 및 구현                           |
