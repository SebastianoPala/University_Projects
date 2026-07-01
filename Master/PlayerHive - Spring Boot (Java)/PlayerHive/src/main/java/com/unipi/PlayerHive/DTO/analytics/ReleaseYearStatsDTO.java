package com.unipi.PlayerHive.DTO.analytics;

import lombok.AllArgsConstructor;
import lombok.Getter;

@Getter
@AllArgsConstructor
public class ReleaseYearStatsDTO {
    private Integer releaseYear;
    private Double avgScore;
    private Integer totalGames;

}
