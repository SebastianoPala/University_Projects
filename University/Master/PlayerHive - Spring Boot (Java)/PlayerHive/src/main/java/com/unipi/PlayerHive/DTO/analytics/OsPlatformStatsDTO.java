package com.unipi.PlayerHive.DTO.analytics;

import lombok.AllArgsConstructor;
import lombok.Getter;

@Getter
@AllArgsConstructor
public class OsPlatformStatsDTO {

    private Integer osCount;
    private Double avgScore;
    private Integer totalGames;

}
